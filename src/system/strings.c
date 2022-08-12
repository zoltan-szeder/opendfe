#include <system/strings.h>
#include <string.h>

long findFirstFrom(char* str, char ch, size_t offset) {
    size_t size = strlen(str);

    for(size_t i = offset; i < size; i++) {
        if(str[i] == ch) {
            return offset;
        }
    }

    return size;
}
