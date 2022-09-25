#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "system/stringbuffer.h"
#include "system/memory.h"

#define STRING_BUFFER_INCREMENT 256

struct StringBuffer {
    size_t length;
    char* cursor;
    char* buffer;
};

size_t stringBufferRemainingSize(StringBuffer* sb);
void stringBufferIncreaseSize(StringBuffer* sb, size_t size);

StringBuffer* stringBufferCreate() {
    StringBuffer* sb = memoryAllocate(sizeof(StringBuffer));
    sb->buffer = memoryAllocate(STRING_BUFFER_INCREMENT);
    sb->cursor = sb->buffer;
    sb->length = STRING_BUFFER_INCREMENT;

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
    size_t remainingSize = stringBufferRemainingSize(sb);

    va_start(args, format);
    int size = vsnprintf(sb->cursor, remainingSize, format, args);
    va_end(args);

    if(size >= remainingSize) {
        stringBufferIncreaseSize(sb, size);
        remainingSize = stringBufferRemainingSize(sb);

        va_start(args, format);
        vsnprintf(sb->cursor, remainingSize, format, args);
        va_end(args);
    }

    sb->cursor += size;
}

size_t stringBufferSize(StringBuffer* sb) {
    return sb->cursor - sb->buffer;
}

size_t stringBufferRemainingSize(StringBuffer* sb) {
    return sb->length - stringBufferSize(sb);
}

void stringBufferIncreaseSize(StringBuffer* sb, size_t size) {
    size_t currentSize = stringBufferSize(sb);
    size_t expectedSize = currentSize + size;
    size_t numberOfIncrements = (expectedSize / STRING_BUFFER_INCREMENT) + 1;
    size_t newSize = numberOfIncrements*STRING_BUFFER_INCREMENT;

    char* newBuffer = memoryAllocate(newSize*(sizeof(char)));
    memcpy(newBuffer, sb->buffer, currentSize);

    memoryRelease(sb->buffer);
    sb->buffer = newBuffer;
    sb->cursor = sb->buffer + currentSize;
    sb->length = newSize;
}
