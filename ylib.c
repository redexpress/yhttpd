#include "ylib.h"
#include <string.h>

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
