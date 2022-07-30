#include <string.h>
#include <stdarg.h>

#include "inmemoryfile.h"
#include "types.h"
#include "stdlib.h"
#include "system/optional.h"
#include "system/memory.h"


long findFirstFrom(char* str, char ch, size_t offset);

OptionalPtr* memFileCreate(char* content, uint32 length) {
    OptionalPtr* optional = memoryAllocate(sizeof(InMemoryFile));
    if(optionalIsEmpty(optional)) return optional;
    InMemoryFile* file = optionalGet(optional);

    file->content = content;
    file->length = length;
    file->pos = 0;

    return optionalOf(file);
}

int32 memFileSeek(InMemoryFile* file, int64 offset, int32 origin) {
    uint64 currentPosition;
    if (origin == SEEK_CUR) currentPosition = file->pos;
    else if (origin == SEEK_SET) currentPosition = 0;
    else currentPosition = file->length;

    currentPosition += offset;
    if(currentPosition > file->length)
        return 1;

    file->pos = currentPosition;
    return 0;
}

void inMemFileDelete(InMemoryFile* file) {
    memoryRelease(file->content);
    memoryRelease(file);
}

bool inMemFileOverEOF(InMemoryFile* file, uint64 length) {
    return file->pos + length > file->length;
}

size_t getBlockLenghtFromFormat(char* format) {
    size_t size = 0;

    for(size_t start = 0; start < strlen(format); ){
        size += atoi(format + start + 2);
        size_t end = findFirstFrom(format, '%', start+1);
        start = end;
    }

    return size;
}

OptionalPtr* inMemFileReadAll(InMemoryFile* file, char* format) {
    size_t size = getBlockLenghtFromFormat(format);

    OptionalPtr* optional = inMemFileRead(file, size);
    if(optionalIsEmpty(optional)) return optional;
    void* object = optionalGet(optional);

    int cursor = 0;
    for(size_t start = 0; start < strlen(format); ){
        size_t end = findFirstFrom(format, '%', start+1);
        char modifier = format[start+1];

        int length = atoi(format + start + 2);
        if(modifier == 'b' && !isBigEndian()) {
            reverseEndianness(object + cursor, length);
        }
        if(modifier == 'l' && !isLittleEndian()) {
            reverseEndianness(object + cursor, length);
        }

        cursor += length;
        start = end;
    }

    return optionalOf(object);
}

long findFirstFrom(char* str, char ch, size_t offset) {
    size_t size = strlen(str);

    for(size_t i = offset; i < size; i++) {
        if(str[i] == ch) {
            return offset;
        }
    }

    return size;
}

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32 length) {
    if(inMemFileOverEOF(file, length))
        return optionalEmpty(
            "inmemoryfile.c:inMemFileRead - Could not read %d bytes from %d offset in a %d long file",
            length, file->pos, file->length
        );

    OptionalPtr* optional = memoryAllocate(length);
    if(optionalIsEmpty(optional)) return optional;

    uint8* bytes = optionalGet(optional);
    memcpy(bytes, file->content + file->pos, length);
    file->pos += length;

    return optionalOf(bytes);
}

OptionalUInt8* inMemFileReadLittleEndianUInt8(InMemoryFile* file) {
    uint8 value;

    OptionalPtr* optionalBytes = inMemFileRead(file, sizeof(value));
    if(optionalIsEmpty(optionalBytes)) return (void*) optionalBytes;

    uint8* ptr = optionalGet(optionalBytes);
    value = *ptr;
    memoryRelease(ptr);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt8(value);
}

OptionalUInt16* inMemFileReadLittleEndianUInt16(InMemoryFile* file) {
    uint16 value;

    OptionalPtr* optionalBytes = inMemFileRead(file, sizeof(value));
    if(optionalIsEmpty(optionalBytes)) return (void*) optionalBytes;

    uint16* ptr = optionalGet(optionalBytes);
    value = *ptr;
    memoryRelease(ptr);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt16(value);
}

OptionalUInt32* inMemFileReadLittleEndianUInt32(InMemoryFile* file) {
    uint32 value;

    OptionalPtr* optionalBytes = inMemFileRead(file, sizeof(value));
    if(optionalIsEmpty(optionalBytes)) return (void*) optionalBytes;

    uint32* ptr = optionalGet(optionalBytes);
    value = *ptr;
    memoryRelease(ptr);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt32(value);
}

OptionalUInt64* inMemFileReadLittleEndianUInt64(InMemoryFile* file) {
    uint64 value;

    OptionalPtr* optionalBytes = inMemFileRead(file, sizeof(value));
    if(optionalIsEmpty(optionalBytes)) return (void*) optionalBytes;

    uint64* ptr = optionalGet(optionalBytes);
    value = *ptr;
    memoryRelease(ptr);

    if(isBigEndian()) reverseEndianness(&value, sizeof(value));

    return optionalUInt64(value);
}
