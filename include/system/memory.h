#ifndef SYSTEM_MEMORY_H
#define SYSTEM_MEMORY_H
#include <stddef.h>
#include "system/optional.h"

OptionalPtr* memoryAllocate(size_t size);
void memoryRelease(void*);

#endif
