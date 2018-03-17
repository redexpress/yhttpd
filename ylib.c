#include "ylib.h"
//#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

int substring(char *dst, const char *src, size_t begin, size_t end) {
    if (end < begin || strlen(src) < end) {
        return 1;
    }
    size_t len = end - begin;
    memcpy(dst, src + begin, len);
    *(dst + len) = '\0';
    return 0;
}
