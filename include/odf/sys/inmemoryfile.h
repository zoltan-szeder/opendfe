#ifndef ODF_SYS_IN_MEMORY_FILE_H
#define ODF_SYS_IN_MEMORY_FILE_H

#include "odf/sys/types/inmemoryfile.h"
#include <stdio.h>
#include "odf/sys/types.h"
#include "odf/sys/types/optional.h"

OptionalPtr* memFileCreate(char* content, uint32_t length);
size_t inMemFileCurrentPosition(InMemoryFile* file);
int32_t memFileSeek(InMemoryFile* file, int64_t offset, int32_t origin);
void inMemFileDelete(InMemoryFile* file);
uint64_t inMemFileSize(InMemoryFile* file);

OptionalPtr* inMemFileRead(InMemoryFile* file, uint32_t length);
OptionalPtr* inMemFileReadStruct(InMemoryFile* file, const char* format);

#endif
