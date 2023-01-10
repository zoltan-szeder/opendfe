#ifndef ODF_SYS_LIST_H
#define ODF_SYS_LIST_H

#include "odf/sys/types/list.h"

#include <stddef.h>
#include "odf/sys/types/optional.h"

List* listCreate(size_t initialSize);
void listDelete(List* list);
size_t listSize(List* list);

void listPut(List* list, size_t index, void* value);
void listAppend(List* list, void* value);
OptionalPtr* listGet(List* list, size_t item);
void listForEach(List* list, void (*func)(void*));

List* listOfArray(size_t n, void* array);

#define ARGS_COUNT(type, ...) (sizeof((type){__VA_ARGS__})/sizeof(type))
#define listOf(...) listOfArray(ARGS_COUNT(void*, __VA_ARGS__), __VA_ARGS__)

#endif

