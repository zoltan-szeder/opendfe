#ifndef ODF_SYS_OPTIONAL_H
#define ODF_SYS_OPTIONAL_H

#include "odf/sys/types/optional.h"

#include <stdio.h>
#include <stdbool.h>
#include "odf/sys/macros.h"
#include "odf/sys/types.h"

void* optionalEmptyMessage(const char* message, ...);
OptionalPtr* optionalOf(void* ptr);

bool optionalIsEmpty(void*);

void* optionalGet(OptionalPtr*);
char* optionalGetMessage(void*);
void optionalPrint(FILE* stream, Optional* optional);

void optionalDelete(void*);

// Helper macros
#define optionalEmpty(...) optionalEmptyMessage(__FILE__ ":L" LINE_NUMBER_AS_STRING ":" __VA_ARGS__)

#define OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(type, name, optional, cleanup, returnValue) \
    OptionalPtr* _optional##name = optional; \
    if(optionalIsEmpty(_optional##name)) { cleanup; return returnValue; } \
    type name = optionalGet(_optional##name);

#define OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(type, name, optional, cleanup) \
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(type, name, optional, cleanup, _optional##name)

#define OPTIONAL_ASSIGN_OR_PASSTROUGH(type, name, optional) \
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(type, name, optional, {})

#define OPTIONAL_ASSIGN_OR_RETURN(type, name, optional, returnValue) \
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(type, name, optional, {}, returnValue)

#endif
