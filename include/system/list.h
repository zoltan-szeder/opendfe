#ifndef SYSTEM_LIST_H
#define SYSTEM_LIST_H

#include "types.h"
#include "system/optional.h"

typedef struct List List;

List* listCreate(uint32 size);
void listDelete(List* list);
uint32 listSize(List* list);

OptionalPtr* listPut(List* list, uint32 index, void* value);
OptionalPtr* listGet(List* list, uint32 item);
void listForEach(List* list, void (*func)(void*));


List* listOf(int n, ...);
List* listOfArray(int n, void* array);
List* listOf1(void* i1);
List* listOf2(void* i1, void* i2);
List* listOf3(void* i1, void* i2, void* i3);
List* listOf4(void* i1, void* i2, void* i3, void* i4);
List* listOf5(void* i1, void* i2, void* i3, void* i4, void* i5);
List* listOf6(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6);
List* listOf7(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6, void* i7);
List* listOf8(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6, void* i7, void* i8);

#endif

