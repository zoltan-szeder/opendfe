#ifndef ODF_SYS_IN_MEMORY_FILE_H
#define ODF_SYS_IN_MEMORY_FILE_H

#include "odf/sys/types/inmemoryfile.h"
#include <stdio.h>
#include "odf/sys/types.h"
#include "odf/sys/optional.h"

OptionalPtr* memFileCreate(char* content, uint32 length);
int32 memFileSeek(InMemoryFile* file, int64 offset, int32 origin);
void inMemFileDelete(InMemoryFile* file);
uint64 inMemFileSize(InMemoryFile* file);

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32 length);
OptionalPtr* inMemFileReadStruct(InMemoryFile* file, const char* format);

#endif
