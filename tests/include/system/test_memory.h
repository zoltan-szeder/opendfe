#ifndef SYSTEM_MEMORY_TEST
#define SYSTEM_MEMORY_TEST

#include "system/memory.h"

typedef struct AllocatedMemory AllocatedMemory;

size_t memoryGetAllocations();

bool memoryIsReferencedBy(void*, void*);

void memoryDump();
int memoryGetRefereeCount(void*);
int memoryGetReferenceCount(void*);
#endif
