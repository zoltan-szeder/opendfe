#include "system/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void (*cleanup)() = NULL;

size_t MEMORY_ALLOCATED = 0;
size_t MEMORY_ALLOCATION_SIZE = 0;
size_t MEMORY_ALLOCATION_MAX_SIZE = 100;

#define MEMORY_NAME_MAX_SIZE 16

typedef struct MemoryAllocation MemoryAllocation;
struct MemoryAllocation {
    char name[MEMORY_NAME_MAX_SIZE];
    void* ptr;
    size_t size;
    bool valid;
    uint32 referenceSize;
    MemoryAllocation** references;
    uint32 refereeSize;
    MemoryAllocation** referees;
};

MemoryAllocation* MEMORY_ALLOCATIONS = NULL;


MemoryAllocation* memoryFindAllocation(void*);
void memoryReleaseAllocation(MemoryAllocation*);
void memoryRegister(void* ptr, size_t size);
bool memoryAllocationContainsValidPointer(MemoryAllocation* alloc, void* ptr);

size_t memoryGetAllocations() {
    return MEMORY_ALLOCATED;
}

void memorySetCleanUp(void (*fun)()) {
    cleanup = fun;
}

void* memoryAllocate(size_t size) {
    if(MEMORY_ALLOCATIONS == NULL) {
        MEMORY_ALLOCATIONS = malloc(MEMORY_ALLOCATION_MAX_SIZE*sizeof(MemoryAllocation));
    }

    void* ptr = malloc(size);
    if(ptr == NULL) {
        fprintf(stderr, "system/memory.h:memoryAllocate - Could not allocate memory");
        if(cleanup != NULL) cleanup();
        exit(1);
    }

    memoryRegister(ptr, size);
    MEMORY_ALLOCATED++;

    return ptr;
}

void* memoryAllocateWithTag(size_t size, const char* tag) {
    void* block = memoryAllocate(size);
    memoryTag(block, tag);
    return block;
}

MemoryAllocation* memoryFindFirstInvalidAllocation() {
    for(size_t i = 0; i < MEMORY_ALLOCATION_SIZE; i++) {
        MemoryAllocation* alloc = &(MEMORY_ALLOCATIONS[i]);
        if(!alloc->valid) return alloc;
    }

    MemoryAllocation* alloc = &(MEMORY_ALLOCATIONS[MEMORY_ALLOCATION_SIZE]);
    MEMORY_ALLOCATION_SIZE++;
    return alloc;
}

void memoryRegister(void* ptr, size_t size){
    MemoryAllocation* alloc = memoryFindFirstInvalidAllocation();
    memset(alloc->name, 0, MEMORY_NAME_MAX_SIZE);
    alloc->ptr = ptr;
    alloc->size = size;
    alloc->valid = true;
    alloc->referenceSize = 0;
    alloc->references = malloc(10*sizeof(MemoryAllocation*));
    alloc->refereeSize = 0;
    alloc->referees = malloc(10*sizeof(MemoryAllocation*));
}

void memoryRelease(void* ptr) {
    MemoryAllocation* alloc = memoryFindAllocation(ptr);
    memoryReleaseAllocation(alloc);
}

void memoryReleaseAll() {
    for(size_t i = 0; i < MEMORY_ALLOCATION_SIZE; i++) {
        memoryReleaseAllocation(&(MEMORY_ALLOCATIONS[i]));
    }

    free(MEMORY_ALLOCATIONS);
    MEMORY_ALLOCATIONS = NULL;
    MEMORY_ALLOCATION_SIZE = 0;
}

int refArrayFindFirst(void** array, int arraySize, void* reference) {
    for(int i = 0; i < arraySize; i++) {
        if(array[i] == reference) return i;
    }

    return -1;
}

void memoryRemoveReferee(MemoryAllocation* reference, MemoryAllocation* referee) {
    int lastIndex = reference->refereeSize - 1;
    int index = refArrayFindFirst((void**) reference->referees, reference->refereeSize, referee);

    if(index < 0) return;

    reference->referees[index] = NULL;
    if(lastIndex >= 0) {
        reference->referees[index] = reference->referees[lastIndex];
    }
    reference->refereeSize--;
}

void memoryRemoveAllReference(MemoryAllocation* alloc) {
    for(int i = 0; i < alloc->referenceSize; i++) {
        memoryRemoveReferee(alloc->references[i], alloc);
        alloc->references[i] = NULL;
    }

    alloc->referenceSize = 0;
}

void memoryRemoveReference(MemoryAllocation* referee, MemoryAllocation* reference) {
    int lastIndex = referee->referenceSize - 1;
    int index = refArrayFindFirst((void**) referee->references, referee->referenceSize, reference);

    if(index < 0) return;

    referee->references[index] = NULL;
    if(lastIndex >= 0) {
        referee->references[index] = reference->references[lastIndex];
    }
    referee->referenceSize--;
}

void memoryRemoveAllReferee(MemoryAllocation* alloc) {
    for(int i = 0; i < alloc->refereeSize; i++) {
        memoryRemoveReference(alloc->referees[i], alloc);
        alloc->referees[i] = NULL;
    }

    alloc->refereeSize = 0;
}

void memoryReleaseAllocation(MemoryAllocation* alloc) {
    if(alloc == NULL || !alloc->valid) return;

    memoryRemoveAllReference(alloc);
    memoryRemoveAllReferee(alloc);

    free(alloc->ptr);
    free(alloc->references);
    free(alloc->referees);

    alloc->valid = false;
    alloc->referenceSize = 0;
    alloc->refereeSize = 0;
    MEMORY_ALLOCATED--;
}

bool memoryIsValid(void* ptr) {
    MemoryAllocation* alloc = memoryFindAllocation(ptr);
    if(alloc == NULL) return false;

    return alloc->valid;
}


void memoryTag(void* ptr, const char* name) {
    MemoryAllocation* alloc = memoryFindAllocation(ptr);
    strncpy(alloc->name, name, MEMORY_NAME_MAX_SIZE - 1);
}

MemoryAllocation* memoryFindAllocation(void* ptr) {
    for(size_t i = 0; i < MEMORY_ALLOCATION_SIZE; i++) {
        MemoryAllocation* alloc = &(MEMORY_ALLOCATIONS[i]);
        if(memoryAllocationContainsValidPointer(alloc, ptr)) {
            return alloc;
        }
    }

    return NULL;
}

bool memoryAllocationContainsValidPointer(MemoryAllocation* alloc, void* ptr) {
    return alloc->ptr <= ptr
        && ptr < alloc->ptr + alloc->size
        && alloc->valid;
}


void memoryAddReference(MemoryAllocation* referee, MemoryAllocation* reference);
void memoryScan(void* ptr) {
    MemoryAllocation* referee = memoryFindAllocation(ptr);
    if(referee == NULL || !referee->valid) return;

    size_t ptrSize = sizeof(void*);
    int lastPtrOffset = referee->size < ptrSize ? 0 : referee->size - ptrSize + 1;
    for(size_t i = 0; i < lastPtrOffset; i++) {

        void* refPtr = *((void**)(ptr + i)); // Assuming object contains a reference

        MemoryAllocation* reference = memoryFindAllocation(refPtr);
        if(reference == NULL || !referee->valid) continue;

        memoryAddReference(referee, reference);
    }
}

void memoryAddReference(MemoryAllocation* referee, MemoryAllocation* reference) {
    referee->references[referee->referenceSize] = reference;
    referee->referenceSize++;
    reference->referees[reference->refereeSize] = referee;
    reference->refereeSize++;
}

bool memoryIsReferencedBy(void* referencedPtr, void* refereePtr) {
    MemoryAllocation* referenced = memoryFindAllocation(referencedPtr);
    MemoryAllocation* referee = memoryFindAllocation(refereePtr);

    for(int i = 0; i < referee->referenceSize; i++) {
        MemoryAllocation* refAlloc = referee->references[i];
        if(refAlloc == referenced) return true;
    }

    return false;
}

void memoryFileDump(FILE* stream);

void memoryDump(){
    memoryFileDump(stderr);
}

void memoryFileDump(FILE* stream){
    uint32 sum = 0;
    for(int i = 0; i < MEMORY_ALLOCATION_SIZE; i++) {
        MemoryAllocation* alloc = &(MEMORY_ALLOCATIONS[i]);

        fprintf(stream, "- name: %-15s\n", alloc->name);
        fprintf(stream, "  ptr: %p\n", alloc->ptr);
        fprintf(stream, "  size: %zu\n", alloc->size);
        fprintf(stream, "  valid: %s\n", alloc->valid ? "true" : "false");
        if(alloc->referenceSize > 0) {
            fprintf(stream, "  ref_to:\n");
            for(int j = 0; j < alloc->referenceSize; j++) {
                MemoryAllocation* refAlloc = alloc->references[j];
                fprintf(stream, "  - %p\n", refAlloc->ptr);
            }
        }

        if(alloc->refereeSize > 0) {
            fprintf(stream, "  ref_by:\n");
            for(int j = 0; j < alloc->refereeSize; j++) {
                MemoryAllocation* refAlloc = alloc->referees[j];
                fprintf(stream, "  - %p\n", refAlloc->ptr);
            }
        }

        if(alloc->valid)
            sum += alloc->size;
    }
    fprintf(stream, "blocks: %zu\n", MEMORY_ALLOCATED);
    fprintf(stream, "total_size: %d\n", sum);
}


int memoryGetRefereeCount(void* obj) {
    MemoryAllocation* alloc = memoryFindAllocation(obj);
    return alloc->refereeSize;
}
int memoryGetReferenceCount(void* obj) {
    MemoryAllocation* alloc = memoryFindAllocation(obj);
    return alloc->referenceSize;
}
