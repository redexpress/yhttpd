#ifndef ylib_h
#define ylib_h

#include <sys/types.h>

int substring(char *dst, const char *src, size_t begin, size_t end);
int base64encode(char *dst, const unsigned char *src, size_t len, size_t *out_len);
int base64decode(unsigned char *dst, const char *src, size_t len, size_t *out_len);

#endif /* ylib_h */
