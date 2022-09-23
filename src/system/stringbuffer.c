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
    StringBuffer* sb = memoryAllocate(sizeof(StringBuffer));
    sb->buffer = memoryAllocate(size);
    sb->cursor = sb->buffer;
    sb->length = size;

    memoryTag(sb, "StringBuffer");
    memoryTag(sb->buffer, "StringBufferArray");
    return sb;
}

void stringBufferDelete(StringBuffer* sb){
    memoryRelease(sb->buffer);
    memoryRelease(sb);
}

char* stringBufferToString(StringBuffer* sb) {
    size_t size = stringBufferSize(sb);

    char* str = memoryAllocate((size + 1)*sizeof(char));
    memoryTag(str, "StringFromBuffer");

    memcpy(str, sb->buffer, size);
    str[size] = 0;

    return str;
}


void stringBufferAppend(StringBuffer* sb, const char* format, ...) {
    va_list args;

    va_start(args, format);
    vsnprintf(sb->buffer, sb->length, format, args);

    va_end(args);
    sb->cursor += 1;

}

size_t stringBufferSize(StringBuffer* sb) {
    return sb->cursor - sb->buffer;
}
