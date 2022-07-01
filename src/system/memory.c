#include "system/memory.h"
#include <stdlib.h>

OptionalPtr* memoryAllocate(size_t size){
    void* ptr = malloc(size);
    if(ptr == NULL) {
        return optionalEmpty("system/memory.h:memoryAllocate - Could not allocate memory");
    } else {
        return optionalOf(ptr);
    }
}

void memoryRelease(void* ptr) {
    free(ptr);
}
