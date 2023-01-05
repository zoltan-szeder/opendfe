#ifndef ODF_SYS_OPTIONAL_H
#define ODF_SYS_OPTIONAL_H

#include "odf/sys/types/optional.h"

#include <stdio.h>
#include "stdbool.h"
#include "odf/sys/types.h"

void* optionalEmpty(const char* message, ...);
bool optionalIsEmpty(void*);
Optional* optionalFirstEmpty(size_t length, ...);
void optionalDelete(void*);
char* optionalGetMessage(void*);
void optionalPrint(FILE* stream, Optional* optional);

OptionalPtr* optionalOf(void* ptr);
void* optionalGet(OptionalPtr*);

#endif
