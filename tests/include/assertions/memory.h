#ifndef ASSERTIONS_MEMORY_H
#define ASSERTIONS_MEMORY_H

#include "test_fixtures.h"
#include "system/test_memory.h"

void assertAllMemoryReleased(){
    if(memoryGetAllocations() > 0) {
        fail(error("Failed to release allocated memory"));
    }
}

#endif
