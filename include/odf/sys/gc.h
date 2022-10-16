#ifndef ODF_SYS_GC_H
#define ODF_SYS_GC_H

#include <stdbool.h>

bool memoryIsValid(void*);
bool memoryIsEmpty();

void memoryReleaseAll();

void memoryScan(void*);
void memoryDump(bool includeInvalid);

#endif
