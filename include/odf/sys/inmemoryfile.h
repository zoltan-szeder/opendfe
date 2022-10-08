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
OptionalPtr* inMemFileReadStruct(InMemoryFile* file, const char* format);

#endif
