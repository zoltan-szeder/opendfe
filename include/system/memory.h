#ifndef SYSTEM_MEMORY_H
#define SYSTEM_MEMORY_H
#include <stddef.h>
#include "system/optional.h"

void* memoryAllocate(size_t size);
void memoryRelease(void*);

bool memoryIsValid(void*);

void memoryScan(void*);

void memoryReleaseAll();

#endif
