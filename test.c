#include "yhttpd.h"
#include "ylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

void testBase64(char *s){
    unsigned char *str = (unsigned char *)s;
    char strout[1000];
    unsigned char ori[1000];
    size_t len;
    int suc = base64encode(strout, str, strlen((const char *)str), &len);
    strout[len] = '\0';
    suc = base64decode(ori, strout, strlen(strout), &len);
    ori[len] = '\0';
    for (int i = 0; i < strlen((const char *)str); i++) {
        assert(ori[i] == str[i]);
    }
}

void testBase64Multiple(){
    testBase64("good");
    testBase64("\x97\x12\x36");
    testBase64("httpd");
    testBase64("yhttpd");
}

void testBase64One(const char *msg, const char *digest){
    uint8_t result[16];
    size_t len = strlen(msg);
    md5((uint8_t*)msg, len, result);
}



void testEncodeHex(){
    char h[] = "0123456789abcdef";
    char str[] = "yang";
    char str2[strlen(str)+1];
    size_t len = strlen(str);
    char hexStr[len * 2 + 1];
    for(int i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)str[i];
        hexStr[i*2] = h[ch / 16];
        hexStr[i*2+1] = h[ch % 16];
    }
    hexStr[len * 2] = '\0';
    int i;
    for(i = 0; i < len; i++) {
        int value = (int)indexOfChar(h, hexStr[2 * i]) * 16;
        value += indexOfChar(h, hexStr[2 * i + 1]);
        str2[i] = value;
    }
    str2[i] = '\0';
    printf("%s\n", hexStr);
    printf("%s\n", str2);
}

void testHex() {
    testEncodeHex();
}

#if TEST == 1
int main() {
    testBase64Multiple();
    testHex();
    printf("All test passed!\n");
    return 0;
}
#endif
