#include <stdarg.h>
#include "odf/sys/list.h"
#include "odf/sys/memory.h"

struct List {
    uint32 size;
    void** values;
};

List* listCreate(uint32 size) {
    List* list = memoryAllocate(sizeof(List));

    list->values = memoryAllocate(size*sizeof(void**));
    list->size = size;

    for(int i = 0; i < list->size; i++) {
        list->values[i] = NULL;
    }

    return list;
}

void listDelete(List* list) {
    memoryRelease(list->values);
    memoryRelease(list);
}

uint32 listSize(List* list) {
    return list->size;
}

OptionalPtr* listGet(List* list, uint32 index) {
    if(index >= list->size) {
        return optionalEmpty("system/list.c:listGet: Array index (%s) is out of bounds (>=%s)", index, list->size);
    }

    return optionalOf(list->values[index]);
}

OptionalPtr* listPut(List* list, uint32 index, void* value) {
    if(index >= list->size) {
        return optionalEmpty("system/list.c:listPut: Array index (%s) is out of bounds (>=%s)", index, list->size);
    }

    list->values[index] = value;

    return optionalOf(value);
}

void listForEach(List* list, void (*func)(void*)) {
    for(int i = 0;  i < list->size; i++) {
        func(list->values[i]);
    }
}

List* listOfVaList(int n, va_list* args) {
    List* list = listCreate(n);

    for(int i = 0; i < n; i++) {
        listPut(list, i, va_arg(*args, void*));
    }

    return list;
}

List* listOf(int n, ...) {
    va_list args;
    va_start(args, n);

    List* list = listOfVaList(n, &args);

    va_end(args);

    return list;
}


List* listOfArray(int n, void* array) {
    List* list = listCreate(n);


    for(int i = 0; i < n; i++) {
        listPut(list, i, array + i);
    }

    return list;
}

List* listOf1(void* i1){
    return listOf(1, i1);
}
List* listOf2(void* i1, void* i2){
    return listOf(2, i1, i2);
}
List* listOf3(void* i1, void* i2, void* i3){
    return listOf(3, i1, i2, i3);
}
List* listOf4(void* i1, void* i2, void* i3, void* i4){
    return listOf(4, i1, i2, i3, i4);
}
List* listOf5(void* i1, void* i2, void* i3, void* i4, void* i5){
    return listOf(5, i1, i2, i3, i4, i5);
}
List* listOf6(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6){
    return listOf(6, i1, i2, i3, i4, i5, i6);
}
List* listOf7(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6, void* i7){
    return listOf(7, i1, i2, i3, i4, i5, i6, i7);
}
List* listOf8(void* i1, void* i2, void* i3, void* i4, void* i5, void* i6, void* i7, void* i8){
    return listOf(8, i1, i2, i3, i4, i5, i6, i7, i8);
}
