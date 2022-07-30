#ifndef IN_MEMORY_FILE_H
#define IN_MEMORY_FILE_H

#include <stdio.h>
#include "types.h"
#include "system/optional.h"

typedef struct {
    uint32 length;
    uint64 pos;
    char* content;
} InMemoryFile;

OptionalPtr* memFileCreate(char* content, uint32 length);
int32 memFileSeek(InMemoryFile* file, int64 offset, int32 origin);
void inMemFileDelete(InMemoryFile* file);

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32 length);
OptionalPtr* inMemFileReadAll(InMemoryFile* file, char* format);


OptionalUInt8* inMemFileReadLittleEndianUInt8(InMemoryFile* file);
OptionalUInt16* inMemFileReadLittleEndianUInt16(InMemoryFile* file);
OptionalUInt32* inMemFileReadLittleEndianUInt32(InMemoryFile* file);
OptionalUInt64* inMemFileReadLittleEndianUInt64(InMemoryFile* file);

OptionalInt8* inMemFileReadLittleEndianInt8(InMemoryFile* file);
OptionalInt16* inMemFileReadLittleEndianInt16(InMemoryFile* file);
OptionalInt32* inMemFileReadLittleEndianInt32(InMemoryFile* file);
OptionalInt64* inMemFileReadLittleEndianInt64(InMemoryFile* file);


OptionalUInt8* inMemFileReadBigEndianUInt8(InMemoryFile* file);
OptionalUInt16* inMemFileReadBigEndianUInt16(InMemoryFile* file);
OptionalUInt32* inMemFileReadBigEndianUInt32(InMemoryFile* file);
OptionalUInt64* inMemFileReadBigEndianUInt64(InMemoryFile* file);

OptionalInt8* inMemFileReadBigEndianInt8(InMemoryFile* file);
OptionalInt16* inMemFileReadBigEndianInt16(InMemoryFile* file);
OptionalInt32* inMemFileReadBigEndianInt32(InMemoryFile* file);
OptionalInt64* inMemFileReadBigEndianInt64(InMemoryFile* file);

#endif
