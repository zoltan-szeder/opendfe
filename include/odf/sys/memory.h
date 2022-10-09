#ifndef ODF_SYS_MEMORY_H
#define ODF_SYS_MEMORY_H

#include <stdbool.h>

void* memoryAllocate(size_t size);
void* memoryAllocateWithTag(size_t size, const char* tag);
void memoryRelease(void*);

bool memoryIsValid(void*);
void memoryTag(void* ptr, const char* name);

void memoryScan(void*);

void memoryReleaseAll();

#endif
