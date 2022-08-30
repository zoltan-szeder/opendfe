#include "system/memory.h"
#include <stdlib.h>
#include <stdio.h>

void (*cleanup)() = NULL;

void memorySetCleanUp(void (*fun)()) {
    cleanup = fun;
}

void* memoryAllocate(size_t size) {
    void* ptr = malloc(size);
    if(ptr == NULL) {
        fprintf(stderr, "system/memory.h:memoryAllocate - Could not allocate memory");
        if(cleanup != NULL) cleanup();
        exit(1);
    }

    return ptr;
}

void memoryRelease(void* ptr) {
    free(ptr);
}
