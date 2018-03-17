#include "ylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int substring(char *dst, const char *src, size_t begin, size_t end) {
    if (end < begin || strlen(src) < end) {
        return 1;
    }
    size_t len = end - begin;
    memcpy(dst, src + begin, len);
    *(dst + len) = '\0';
    return 0;
}

ssize_t indexOfChar(const char *big, char c) {
    ssize_t i = 0;
    for (i = 0; i < strlen(big); i++) {
        if (big[i] == c) return i;
    }
    return -1;
}

int indexOf(const char *big, const char *little) {
    const char *p = big;
    const char *q = big;
    p = strstr(big, little);
    if (p == NULL) {
        return -1;
    }
    int i = 0;
    while (q != p) {
        q++;
        i++;
    }
    return i;
}

int base64encode(char *dst, const unsigned char *src, size_t len, size_t *out_len) {
    const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 3, j++) {
        size_t remain = len - 3 * j;
        dst[j * 4 + 0] = base64char[* (src + i) >> 2];
        if (remain < 3) {
            if (remain == 1) {
                dst[j * 4 + 1] = base64char[(*(src + i) & 3) << 4];
                dst[j * 4 + 2] = '=';
            } else {
                dst[j * 4 + 1] = base64char[((*(src + i) & 3) << 4) | (*(src + i + 1) >> 4)];
                dst[j * 4 + 2] = base64char[((*(src + i + 1) & 0x0f) << 2)];
            }
            dst[j * 4 + 3] = '=';
        } else {
            dst[j * 4 + 1] = base64char[((*(src + i) & 3) << 4) | (*(src + i + 1) >> 4)];
            dst[j * 4 + 2] = base64char[((*(src + i + 1) & 0x0f) << 2) | (*(src + i + 2) >> 6)];
            dst[j * 4 + 3] = base64char[*(src + i + 2) & 0x3f];
        }
    }
    *out_len = j * 4;
    
    return 0;
}

int base64decode(unsigned char *dst, const char *src, size_t len, size_t *out_len) {
    const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int equalSignCount = 0;
    if (len % 4 == 0) {
        if (src[len-2] == '=') {
            equalSignCount = 2;
        } else if (src[len-1] == '=') {
            equalSignCount = 1;
        }
    } else if (len % 4 == 1) {
        return 1;
    } else if (len % 4 == 2) {
        equalSignCount = 2;
    } else if (len % 4 == 3) {
        equalSignCount = 1;
    }
    size_t i, j;
    for ( i = 0, j = 0; i < len - equalSignCount; i += 4, j++) {
        dst[j * 3] = (indexOfChar(base64char, src[i]) << 2) | (indexOfChar(base64char, src[i + 1]) >> 4);
        size_t remain = len - 4 * i;
        if (remain < 4) {
            if (equalSignCount == 1) {
                dst[i + 1] = ((indexOfChar(base64char, src[i + 1]) & 0xf) << 4) | (indexOfChar(base64char, src[i + 2]) >> 2);
                break;
            } else if (equalSignCount == 2) {
                break;
            }
        }
        dst[j * 3 + 1] = ((indexOfChar(base64char, src[i + 1]) & 0xf) << 4) | (indexOfChar(base64char, src[i + 2]) >> 2);
        dst[j * 3 + 2] = (indexOfChar(base64char, src[i + 2]) >> 4 << 6) | indexOfChar(base64char, src[i + 3]);
    }
    *out_len = j * 3 - equalSignCount;
    return 0;
}

void toBytes(uint32_t val, uint8_t *bytes) {
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}

uint32_t toInt32(const uint8_t *bytes) {
    return ((uint32_t)bytes[0])        | ((uint32_t)bytes[1] << 8)
    | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

#define leftrotate(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest) {
    const uint32_t s[64] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};
    const uint32_t k[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
    uint32_t a0 = 0x67452301;   //a
    uint32_t b0 = 0xefcdab89;   //b
    uint32_t c0 = 0x98badcfe;   //d
    uint32_t d0 = 0x10325476;   //d
    size_t new_len;
    for (new_len = initial_len + 1; new_len % (512/8) != 448/8; new_len++) {
    }
    uint8_t *msg = (uint8_t *)malloc(new_len + 8);
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80;
    for (size_t offset = initial_len + 1; offset < new_len; offset++) {
        msg[offset] = 0;
    }
    toBytes((uint32_t)initial_len * 8, msg + new_len);
    toBytes((uint32_t)initial_len >> 29, msg + new_len + 4);
    for(size_t offset = 0; offset < new_len; offset += (512/8)) {
        uint32_t m[16];
        for(int i = 0; i < 16; i++) {
            m[i] = toInt32(msg + offset + i * 4);
        }
        uint32_t a = a0;
        uint32_t b = b0;
        uint32_t c = c0;
        uint32_t d = d0;
        for(int i = 0; i < 64; i++) {
            uint32_t f, g;
            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }
            f = f + a + k[i] + m[g];
            a = d;
            d = c;
            c = b;
            b = b + leftrotate(f, s[i]);
        }
        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }
    free(msg);
    toBytes(a0, digest);
    toBytes(b0, digest + 4);
    toBytes(c0, digest + 8);
    toBytes(d0, digest + 12);
}
