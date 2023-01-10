#include "system/test_memory.h"
#include "test_fixtures.h"

typedef struct ObjPtrFirst {
    void* ptr;
    char c;
} ObjPtrFirst;

typedef struct ObjPtrMiddle {
    char c1;
    void* ptr;
    char c2;
} ObjPtrMiddle;

typedef struct ObjPtrLast {
    char c;
    void* ptr;
} ObjPtrLast;

void tearDown(){
    memoryReleaseAll();
}

void testMemoryInitialization() {
    testCase("testMemoryInitialization");

    assertEquali(0, memoryGetAllocations());
}

void testMemoryWithOneAllocation() {
    testCase("testMemoryWithOneAllocation");

    void* memoryBlock = memoryAllocate(100);

    assertEquali(1, memoryGetAllocations());
    assertNotNull(memoryBlock);
    assertTrue(memoryIsValid(memoryBlock));

    memoryRelease(memoryBlock);
}

void testMemoryBoundaries() {
    testCase("testMemoryBoundaries");

    void* memoryBlock1 = memoryAllocate(1);
    void* memoryBlock2 = memoryAllocate(100);

    assertFalse(memoryIsValid(memoryBlock1-1));
    assertTrue(memoryIsValid(memoryBlock1));
    assertFalse(memoryIsValid(memoryBlock1+1));

    assertFalse(memoryIsValid(memoryBlock2-1));
    assertTrue(memoryIsValid(memoryBlock2));
    assertTrue(memoryIsValid(memoryBlock2+1));
    assertTrue(memoryIsValid(memoryBlock2+50));
    assertTrue(memoryIsValid(memoryBlock2+99));
    assertFalse(memoryIsValid(memoryBlock2+100));

    memoryReleaseAll();
}

void testMemoryRelease() {
    testCase("testMemoryRelease");

    void* memoryBlock = memoryAllocate(100);
    memoryRelease(memoryBlock);
    assertFalse(memoryIsValid(memoryBlock));

    assertEquali(0, memoryGetAllocations());
}

void testMemoryReleaseWithOffset() {
    testCase("testMemoryReleaseWithOffset");

    void* memoryBlock = memoryAllocate(100);
    memoryRelease(memoryBlock + 50);
    assertFalse(memoryIsValid(memoryBlock));

    assertEquali(0, memoryGetAllocations());
}

void testMemoryReleaseOutOfOrder() {
    testCase("testMemoryReleaseOutOfOrder");

    void* memoryBlock1 = memoryAllocate(100);
    void* memoryBlock2 = memoryAllocate(100);

    memoryRelease(memoryBlock1);

    assertFalseMsg(memoryIsValid(memoryBlock1), error("memoryBlock1 is valid"));
    assertTrueMsg(memoryIsValid(memoryBlock2), error("memoryBlock2 is invalid"));
    assertEquali(1, memoryGetAllocations());
}

void testReleaseAll() {
    testCase("testReleaseAll");

    void* memoryBlock = memoryAllocate(100);
    memoryReleaseAll();

    assertEquali(0, memoryGetAllocations());
    assertFalse(memoryIsValid(memoryBlock));
}

void testReferences() {
    testCase("testReleaseAll");

    void* memoryBlock = memoryAllocate(1);
    ObjPtrFirst* ptrFirst = memoryAllocate(sizeof(ObjPtrFirst));
    ObjPtrMiddle* ptrMiddle = memoryAllocate(sizeof(ObjPtrMiddle));
    ObjPtrLast* ptrLast = memoryAllocate(sizeof(ObjPtrLast));

    ptrFirst->ptr = memoryBlock;
    ptrMiddle->ptr = memoryBlock;
    ptrLast->ptr = memoryBlock;

    memoryScan(memoryBlock);
    memoryScan(ptrFirst);
    memoryScan(ptrMiddle);
    memoryScan(ptrLast);

    assertEquali(3, memoryGetRefereeCount(memoryBlock));
    assertEquali(1, memoryGetReferenceCount(ptrFirst));
    assertEquali(1, memoryGetReferenceCount(ptrMiddle));
    assertEquali(1, memoryGetReferenceCount(ptrLast));

    assertFalseMsg(memoryIsReferencedBy(ptrFirst, memoryBlock), error("memoryBlock contains reference to ptrFirst"));
    assertTrueMsg(memoryIsReferencedBy(memoryBlock, ptrFirst), error("memoryBlock is not referenced by ptrFirst"));

    assertFalseMsg(memoryIsReferencedBy(ptrMiddle, memoryBlock), error("memoryBlock contains reference to ptrMiddle"));
    assertTrueMsg(memoryIsReferencedBy(memoryBlock, ptrMiddle), error("memoryBlock is not referenced by ptrMiddle"));

    assertFalseMsg(memoryIsReferencedBy(ptrLast, memoryBlock), error("memoryBlock contains reference to ptrLast"));
    assertTrueMsg(memoryIsReferencedBy(memoryBlock, ptrLast), error("memoryBlock is not referenced by ptrLast"));
}

void testReferenceReplacement() {
    testCase("testReferenceReplacement");

    void* memoryBlock = memoryAllocate(1);
    ObjPtrFirst* ptr1 = memoryAllocate(sizeof(ObjPtrFirst));
    ObjPtrFirst* ptr2 = memoryAllocate(sizeof(ObjPtrFirst));
    ObjPtrFirst* ptr3 = memoryAllocate(sizeof(ObjPtrFirst));

    ptr1->ptr = memoryBlock;
    ptr2->ptr = memoryBlock;
    ptr3->ptr = memoryBlock;

    memoryScan(memoryBlock);
    memoryScan(ptr1);
    memoryScan(ptr2);
    memoryScan(ptr3);

    memoryRelease(ptr2);

    assertEquali(2, memoryGetRefereeCount(memoryBlock));
    assertTrueMsg(memoryIsReferencedBy(memoryBlock, ptr1), error("memoryBlock is not referenced by ptr1"));
    assertTrueMsg(memoryIsReferencedBy(memoryBlock, ptr3), error("memoryBlock is not referenced by ptr3"));
}



void testReallocation() {
    testCase("testReallocation");

    ObjPtrFirst* allocated = memoryAllocate(sizeof(ObjPtrFirst));
    allocated->c = 1;
    ObjPtrFirst* reallocated = memoryReallocate(allocated, 2*sizeof(ObjPtrFirst));
    reallocated[1].c = 2;

    assertEquali(1, reallocated[0].c);
    assertEquali(2, reallocated[1].c);
    assertFalse(memoryIsValid(allocated));
    assertTrue(memoryIsValid(reallocated));
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testMemoryInitialization,
        &testMemoryWithOneAllocation,
        &testMemoryRelease,
        &testMemoryReleaseWithOffset,
        &testMemoryReleaseOutOfOrder,
        &testMemoryBoundaries,
        &testReleaseAll,
        &testReferences,
        &testReferenceReplacement,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/memory.c";
    fixture.after = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    return runTests(&fixture);
}
