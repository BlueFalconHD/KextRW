#include "utils.h"
#include <libkextrw.h>

#include <IOKit/IOKitLib.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach/arm/vm_param.h>
#include <mach/kern_return.h>
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/thread_act.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  if (kextrw_init() == -1) {
    printf("Failed to initialize KextRW\n");
    return 1;
  }

  uint64_t kernelBase = get_kernel_base();
  if (kernelBase == 0) {
    printf("Failed to get kernel base\n");
    kextrw_deinit();
    return 1;
  }

  printf("Kernel base: 0x%llx\n", kernelBase);

  // Read the Mach-O header at the kernel base address
  struct mach_header_64 header;
  if (kreadbuf(kernelBase, &header, sizeof(header)) != KERN_SUCCESS) {
    printf("Failed to read Mach-O header\n");
    kextrw_deinit();
    return 1;
  }

  // Verify Mach-O magic
  if (header.magic != MH_MAGIC_64) {
    printf("Invalid Mach-O magic number: 0x%X\n", header.magic);
    kextrw_deinit();
    return 1;
  }

  size_t lc_size = header.sizeofcmds;
  uint8_t *lc_buffer = malloc(lc_size);
  if (!lc_buffer) {
    printf("Failed to allocate memory for load commands\n");
    kextrw_deinit();
    return 1;
  }

  uint64_t lc_address = kernelBase + sizeof(struct mach_header_64);
  if (kreadbuf(lc_address, lc_buffer, lc_size) != KERN_SUCCESS) {
    printf("Failed to read load commands\n");
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  uint64_t current_file_offset = sizeof(struct mach_header_64) + lc_size;

  FILE *output_file = fopen("kernel_dump", "wb");
  if (!output_file) {
    printf("Failed to open output file\n");
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  if (fseek(output_file, current_file_offset, SEEK_SET) != 0) {
    printf("Failed to seek in output file\n");
    fclose(output_file);
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  uint64_t offset = 0;
  for (uint32_t i = 0; i < header.ncmds; i++) {
    if (offset + sizeof(struct load_command) > lc_size) {
      printf("Load commands malformed or incomplete\n");
      break;
    }
    struct load_command *lc = (struct load_command *)(lc_buffer + offset);

    if (lc->cmd == LC_SEGMENT_64) {
      struct segment_command_64 *seg_cmd =
          (struct segment_command_64 *)(lc_buffer + offset);

      // Read the segment data from memory
      uint64_t segment_start = seg_cmd->vmaddr;
      uint64_t segment_size = seg_cmd->vmsize;
      uint64_t file_size = seg_cmd->filesize;

      if (file_size > 0) {
        uint8_t *segment_data = malloc(file_size);
        if (!segment_data) {
          printf("Failed to allocate memory for segment %s\n",
                 seg_cmd->segname);
          seg_cmd->filesize = 0;
          seg_cmd->fileoff = 0;
          offset += lc->cmdsize;
          continue;
        }

        kern_return_t kr = kreadbuf(segment_start, segment_data, file_size);
        if (kr != KERN_SUCCESS) {
          printf("Failed to read segment %s from memory: %s (0x%llx)\n",
                 seg_cmd->segname, get_error_name(kr), kr);
          free(segment_data);
          seg_cmd->filesize = 0;
          seg_cmd->fileoff = 0;
          offset += lc->cmdsize;
          continue;
        }

        seg_cmd->fileoff = current_file_offset;

        if (fseek(output_file, current_file_offset, SEEK_SET) != 0) {
          printf("Failed to seek in output file\n");
          free(segment_data);
          fclose(output_file);
          free(lc_buffer);
          kextrw_deinit();
          return 1;
        }

        if (fwrite(segment_data, file_size, 1, output_file) != 1) {
          printf("Failed to write segment %s to file\n", seg_cmd->segname);
          free(segment_data);
          fclose(output_file);
          free(lc_buffer);
          kextrw_deinit();
          return 1;
        }

        free(segment_data);

        current_file_offset += file_size;
      } else {
        seg_cmd->fileoff = 0;
      }
    }

    offset += lc->cmdsize;
  }

  if (fseek(output_file, 0, SEEK_SET) != 0) {
    printf("Failed to seek to beginning of output file\n");
    fclose(output_file);
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  if (fwrite(&header, sizeof(header), 1, output_file) != 1) {
    printf("Failed to write Mach-O header to file\n");
    fclose(output_file);
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  if (fwrite(lc_buffer, lc_size, 1, output_file) != 1) {
    printf("Failed to write load commands to file\n");
    fclose(output_file);
    free(lc_buffer);
    kextrw_deinit();
    return 1;
  }

  fclose(output_file);
  free(lc_buffer);
  kextrw_deinit();

  printf("Kernel Mach-O dump completed successfully.\n");
  return 0;
}
