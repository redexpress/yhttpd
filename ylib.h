#ifndef ylib_h
#define ylib_h

#include <sys/types.h>
#include <stdint.h>

int substring(char *dst, const char *src, size_t begin, size_t end);
int base64encode(char *dst, const unsigned char *src, size_t len, size_t *out_len);
int base64decode(unsigned char *dst, const char *src, size_t len, size_t *out_len);
ssize_t indexOfChar(const char *big, char c);
void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

#endif /* ylib_h */
