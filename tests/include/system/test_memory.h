#ifndef SYSTEM_MEMORY_TEST
#define SYSTEM_MEMORY_TEST

#include <stdio.h>
#include "odf/sys/memory.h"
#include "test_fixtures.h"

size_t memoryGetAllocations();

bool memoryIsReferencedBy(void*, void*);

void memoryDump();
void memoryFileDump(FILE* stream);

int memoryGetRefereeCount(void*);
int memoryGetReferenceCount(void*);

#endif
