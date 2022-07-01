#include <string.h>

#include "inmemoryfile.h"
#include "types.h"
#include "system/optional.h"
#include "system/memory.h"

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