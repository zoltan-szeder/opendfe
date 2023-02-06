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

int tearDown(void** state){
    memoryReleaseAll();
    return 0;
}

void testMemoryInitialization() {
    assert_int_equal(0, memoryGetAllocations());
}

void testMemoryWithOneAllocation() {
    void* memoryBlock = memoryAllocate(100);

    assert_int_equal(1, memoryGetAllocations());
    assert_non_null(memoryBlock);
    assert_true(memoryIsValid(memoryBlock));

    memoryRelease(memoryBlock);
}

void testMemoryBoundaries() {
    void* memoryBlock1 = memoryAllocate(1);
    void* memoryBlock2 = memoryAllocate(100);

    assert_false(memoryIsValid(memoryBlock1-1));
    assert_true(memoryIsValid(memoryBlock1));
    assert_false(memoryIsValid(memoryBlock1+1));

    assert_false(memoryIsValid(memoryBlock2-1));
    assert_true(memoryIsValid(memoryBlock2));
    assert_true(memoryIsValid(memoryBlock2+1));
    assert_true(memoryIsValid(memoryBlock2+50));
    assert_true(memoryIsValid(memoryBlock2+99));
    assert_false(memoryIsValid(memoryBlock2+100));

    memoryReleaseAll();
}

void testMemoryRelease() {
    void* memoryBlock = memoryAllocate(100);
    memoryRelease(memoryBlock);
    assert_false(memoryIsValid(memoryBlock));

    assert_int_equal(0, memoryGetAllocations());
}

void testMemoryReleaseWithOffset() {
    void* memoryBlock = memoryAllocate(100);
    memoryRelease(memoryBlock + 50);
    assert_false(memoryIsValid(memoryBlock));

    assert_int_equal(0, memoryGetAllocations());
}

void testMemoryReleaseOutOfOrder() {
    void* memoryBlock1 = memoryAllocate(100);
    void* memoryBlock2 = memoryAllocate(100);

    memoryRelease(memoryBlock1);

    assert_false(memoryIsValid(memoryBlock1));
    assert_true(memoryIsValid(memoryBlock2));
    assert_int_equal(1, memoryGetAllocations());
}

void testReleaseAll() {
    void* memoryBlock = memoryAllocate(100);
    memoryReleaseAll();

    assert_int_equal(0, memoryGetAllocations());
    assert_false(memoryIsValid(memoryBlock));
}

void testReferences() {
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

    assert_int_equal(3, memoryGetRefereeCount(memoryBlock));
    assert_int_equal(1, memoryGetReferenceCount(ptrFirst));
    assert_int_equal(1, memoryGetReferenceCount(ptrMiddle));
    assert_int_equal(1, memoryGetReferenceCount(ptrLast));

    assert_false(memoryIsReferencedBy(ptrFirst, memoryBlock));
    assert_true(memoryIsReferencedBy(memoryBlock, ptrFirst));

    assert_false(memoryIsReferencedBy(ptrMiddle, memoryBlock));
    assert_true(memoryIsReferencedBy(memoryBlock, ptrMiddle));

    assert_false(memoryIsReferencedBy(ptrLast, memoryBlock));
    assert_true(memoryIsReferencedBy(memoryBlock, ptrLast));
}

void testReferenceReplacement() {
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

    assert_int_equal(2, memoryGetRefereeCount(memoryBlock));
    assert_true(memoryIsReferencedBy(memoryBlock, ptr1));
    assert_true(memoryIsReferencedBy(memoryBlock, ptr3));
}



void testReallocation() {
    ObjPtrFirst* allocated = memoryAllocate(sizeof(ObjPtrFirst));
    allocated->c = 1;
    ObjPtrFirst* reallocated = memoryReallocate(allocated, 2*sizeof(ObjPtrFirst));
    reallocated[1].c = 2;

    assert_int_equal(1, reallocated[0].c);
    assert_int_equal(2, reallocated[1].c);
    assert_false(memoryIsValid(allocated));
    assert_true(memoryIsValid(reallocated));
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testMemoryInitialization, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testMemoryWithOneAllocation, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testMemoryRelease, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testMemoryReleaseWithOffset, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testMemoryReleaseOutOfOrder, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testMemoryBoundaries, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testReleaseAll, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testReferences, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testReferenceReplacement, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/memory.c", tests, NULL, NULL);

    return ret;
}
