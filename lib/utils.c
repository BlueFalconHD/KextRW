#include "utils.h"

#include <mach/kern_return.h>
#include <mach/mach.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

const char *get_error_name(uint64_t error_code) {
  static const char *error_names[] = {
      "KERN_SUCCESS",                  // 0
      "KERN_INVALID_ADDRESS",          // 1
      "KERN_PROTECTION_FAILURE",       // 2
      "KERN_NO_SPACE",                 // 3
      "KERN_INVALID_ARGUMENT",         // 4
      "KERN_FAILURE",                  // 5
      "KERN_RESOURCE_SHORTAGE",        // 6
      "KERN_NOT_RECEIVER",             // 7
      "KERN_NO_ACCESS",                // 8
      "KERN_MEMORY_FAILURE",           // 9
      "KERN_MEMORY_ERROR",             // 10
      "KERN_ALREADY_IN_SET",           // 11
      "KERN_NOT_IN_SET",               // 12
      "KERN_NAME_EXISTS",              // 13
      "KERN_ABORTED",                  // 14
      "KERN_INVALID_NAME",             // 15
      "KERN_INVALID_TASK",             // 16
      "KERN_INVALID_RIGHT",            // 17
      "KERN_INVALID_VALUE",            // 18
      "KERN_UREFS_OVERFLOW",           // 19
      "KERN_INVALID_CAPABILITY",       // 20
      "KERN_RIGHT_EXISTS",             // 21
      "KERN_INVALID_HOST",             // 22
      "KERN_MEMORY_PRESENT",           // 23
      "KERN_MEMORY_DATA_MOVED",        // 24
      "KERN_MEMORY_RESTART_COPY",      // 25
      "KERN_INVALID_PROCESSOR_SET",    // 26
      "KERN_POLICY_LIMIT",             // 27
      "KERN_INVALID_POLICY",           // 28
      "KERN_INVALID_OBJECT",           // 29
      "KERN_ALREADY_WAITING",          // 30
      "KERN_DEFAULT_SET",              // 31
      "KERN_EXCEPTION_PROTECTED",      // 32
      "KERN_INVALID_LEDGER",           // 33
      "KERN_INVALID_MEMORY_CONTROL",   // 34
      "KERN_INVALID_SECURITY",         // 35
      "KERN_NOT_DEPRESSED",            // 36
      "KERN_TERMINATED",               // 37
      "KERN_LOCK_SET_DESTROYED",       // 38
      "KERN_LOCK_UNSTABLE",            // 39
      "KERN_LOCK_OWNED",               // 40
      "KERN_LOCK_OWNED_SELF",          // 41
      "KERN_SEMAPHORE_DESTROYED",      // 42
      "KERN_RPC_SERVER_TERMINATED",    // 43
      "KERN_RPC_TERMINATE_ORPHAN",     // 44
      "KERN_RPC_CONTINUE_ORPHAN",      // 45
      "KERN_NOT_SUPPORTED",            // 46
      "KERN_NODE_DOWN",                // 47
      "KERN_NOT_WAITING",              // 48
      "KERN_OPERATION_TIMED_OUT",      // 49
      "KERN_CODESIGN_ERROR",           // 50
      "KERN_POLICY_STATIC",            // 51
      "KERN_INSUFFICIENT_BUFFER_SIZE", // 52
      "KERN_DENIED",                   // 53
      "KERN_MISSING_KC",               // 54
      "KERN_INVALID_KC",               // 55
      "KERN_NOT_FOUND"                 // 56
  };

  static const uint64_t max_error_code =
      sizeof(error_names) / sizeof(error_names[0]) - 1;

  if (error_code <= max_error_code) {
    return error_names[error_code];
  } else {
    return "Unknown Error Code";
  }
}

void print_hexdump_of_buf_with_ascii(unsigned char *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (i % 16 == 0) {
      if (i != 0) {
        printf("  ");
        for (size_t j = i - 16; j < i; j++) {
          if (buf[j] >= 0x20 && buf[j] <= 0x7E) {
            printf("%c", buf[j]);
          } else {
            printf(".");
          }
        }
        printf("\n");
      }
      printf("%08lx: ", (unsigned long)(i));
    }
    printf("%02x ", buf[i]);
  }
  printf("\n");
}
