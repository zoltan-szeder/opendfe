#include <system/strings.h>
#include <string.h>
#include "stdlib.h"

long findFirstFrom(const char* str, char ch, size_t offset) {
    size_t size = strlen(str);

    for(size_t i = offset; i < size; i++) {
        if(str[i] == ch) {
            return offset;
        }
    }

    return size;
}


size_t getBlockLenghtFromFormat(const char* format) {
    size_t size = 0;

    for(size_t start = 0; start < strlen(format); ){
        size += atoi(format + start + 2);
        size_t end = findFirstFrom(format, '%', start+1);
        start = end;
    }

    return size;
}
