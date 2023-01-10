#include <stdarg.h>
#include <stddef.h>
#include "odf/sys/list.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"

#define LIST_INCREMENT 32

struct List {
    size_t bufferSize;
    size_t size;
    void** values;
};

static void listExtendToSize(List* list, size_t size);

List* listCreate(size_t initialSize) {
    List* list = memoryAllocateWithTag(sizeof(List), "odf/sys/list/List");
    list->size = 0;
    list->bufferSize = initialSize;

    if(initialSize == 0) {
        list->values = NULL;
        return list;
    }

    list->values = memoryAllocateWithTag(initialSize*sizeof(void**), "odf/sys/list/List/values");

    for(size_t i = 0; i < initialSize; i++) {
        list->values[i] = NULL;
    }

    return list;
}

void listAppend(List* list, void* item) {
    listExtendToSize(list, list->size + 1);

    list->values[list->size] = item;
    list->size++;
}

void listDelete(List* list) {
    memoryRelease(list->values);
    memoryRelease(list);
}

size_t listSize(List* list) {
    return list->size;
}

OptionalPtr* listGet(List* list, size_t index) {
    if(index >= list->size) {
        return optionalEmpty("listGet - Array index (%ld) is out of bounds (>=%ld)", index, list->size);
    }

    return optionalOf(list->values[index]);
}

void listPut(List* list, size_t index, void* value) {
    size_t requiredSize = index+1;
    listExtendToSize(list, requiredSize);

    if(list->size < requiredSize)
        list->size = requiredSize;

    list->values[index] = value;
}

void listForEach(List* list, void (*func)(void*)) {
    for(size_t i = 0;  i < list->size; i++) {
        func(list->values[i]);
    }
}

List* listOfVaList(size_t n, va_list* args) {
    List* list = listCreate(n);

    for(size_t i = 0; i < n; i++) {
        listAppend(list, va_arg(*args, void*));
    }

    return list;
}

List* listOfArray(size_t n, void* array) {
    List* list = listCreate(n);


    for(size_t i = 0; i < n; i++) {
        listAppend(list, array + i);
    }

    return list;
}


// Private functions
static void listExtendToSize(List* list, size_t size){
    if(size <= list->bufferSize) return;

    size_t oldBuffSize = list->bufferSize;
    size_t incBuffSize = oldBuffSize + LIST_INCREMENT;
    size_t newBuffSize = size > incBuffSize ? size : incBuffSize;

    if(oldBuffSize == 0) {
        list->values = memoryAllocateWithTag(newBuffSize*sizeof(void*), "odf/sys/list/List.values");
    } else {
        list->values = memoryReallocate(list->values, newBuffSize*sizeof(void*));
    }

    for(int i = oldBuffSize; i < newBuffSize; i++) {
        list->values[i] = NULL;
    }

    list->bufferSize = newBuffSize;

}

