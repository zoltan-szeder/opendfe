#ifndef ODF_SYS_MEMORY_H
#define ODF_SYS_MEMORY_H

#include <stddef.h>

void* memoryAllocate(size_t size);
void* memoryAllocateWithTag(size_t size, const char* tag);
void memoryTag(void* ptr, const char* name);
void memoryRelease(void*);

#endif
