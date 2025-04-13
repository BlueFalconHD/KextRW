#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

const char *get_error_name(uint64_t error_code);
void print_hexdump_of_buf_with_ascii(unsigned char *buf, size_t len);

#endif /* UTILS_H */
