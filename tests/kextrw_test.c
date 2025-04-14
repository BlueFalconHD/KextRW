#include <libkextrw.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  printf("KextRW test");

  if (kextrw_init() == -1) {
    printf("Failed to initialize KextRW. Is the kernel extension loaded?\n");
    return 1;
  }

  uint64_t kernelBase = get_kernel_base();
  if (kernelBase == 0) {
    printf("Failed to get kernel base address. Make sure SIP is disabled.\n");
    kextrw_deinit();
    return 1;
  }

  printf("Kernel base found: 0x%llx\n", kernelBase);

  printf("From here, you can patch kernel functionality using kslide in "
         "conjunction with offsets found in your kernelcache (the path is at "
         "the top of the output of `kmutil inspect`)\n");

  kextrw_deinit();
  return 0;
}
