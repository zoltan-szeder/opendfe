#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "system/stringbuffer.h"
#include "system/memory.h"

struct StringBuffer {
    size_t length;
    char* cursor;
    char* buffer;
};


StringBuffer* stringBufferCreate(size_t size) {
    size_t sizeWithTerminator = size + 1;
    StringBuffer* sb = memoryAllocate(sizeof(StringBuffer));
    sb->buffer = memoryAllocate(sizeWithTerminator);
    sb->cursor = sb->buffer;
    sb->length = sizeWithTerminator;

    return sb;
}

void stringBufferDelete(StringBuffer* sb){
    memoryRelease(sb->buffer);
    memoryRelease(sb);
}

char* stringBufferToString(StringBuffer* sb) {
    size_t size = stringBufferSize(sb);

    char* str = memoryAllocate((size + 1)*sizeof(char));

    memcpy(str, sb->buffer, size);
    str[size] = 0;

    return str;
}


void stringBufferAppend(StringBuffer* sb, const char* format, ...) {
    va_list args;
    size_t remainingSize = sb->length - stringBufferSize(sb);

    va_start(args, format);
    int size = vsnprintf(sb->cursor, remainingSize, format, args);

    va_end(args);
    sb->cursor += size < remainingSize ? size : remainingSize - 1;
}

size_t stringBufferSize(StringBuffer* sb) {
    return sb->cursor - sb->buffer;
}
