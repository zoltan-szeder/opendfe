#include "test_fixtures.h"
#include "system/test_memory.h"

#include "odf/sys/list.h"

#include <stdint.h>
#include "assertions/optional.h"

List* list;

int tearDown(void** state) {
    listDelete(list);
    assert_true(memoryGetAllocations() == 0);

    return 0;
}

typedef struct {
    uint32_t value;
} MyStruct;

void testEmptyList() {
    list = listCreate(0);

    assert_int_equal(0, listSize(list));
    assertOptionalIsEmpty(listGet(list, 0));
}

void testAppendToEmptyList() {
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listAppend(list, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));

    assert_int_equal(ms.value, msPtr->value);
}

void testAppendToList() {
    MyStruct ms1 = {
        .value = 1
    };
    MyStruct ms2 = {
        .value = 2
    };

    list = listCreate(1);
    listAppend(list, &ms1);
    listAppend(list, &ms2);

    MyStruct* ms1Ptr = optionalGet(listGet(list, 0));
    MyStruct* ms2Ptr = optionalGet(listGet(list, 1));

    assert_int_equal(ms1.value, ms1Ptr->value);
    assert_int_equal(ms2.value, ms2Ptr->value);
}

void testOneMissingItemList() {
    list = listCreate(1);

    assertOptionalIsEmpty(listGet(list, 0));
    assertOptionalIsEmpty(listGet(list, 1));

    assert_int_equal(0, listSize(list));
}

void testOneItemList() {
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(1);
    listPut(list, 0, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assert_int_equal(ms.value, msPtr->value);
}

void testListPutIncreasesListSize() {
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listPut(list, 0, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assert_int_equal(ms.value, msPtr->value);
}
void testListBufferIncreaseWillKeepUnpopulatedEntriesEmpty() {
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listPut(list, 10, &ms);

    assertOptionalIsEmpty(listGet(list, 0));

    MyStruct* msPtr = optionalGet(listGet(list, 10));
    assert_int_equal(ms.value, msPtr->value);
}

void testListOfOne() {

    MyStruct ms = {
        .value = 3
    };

    list = listOf(&ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assert_int_equal(3, msPtr->value);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testEmptyList, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testAppendToEmptyList, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testAppendToList, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testOneMissingItemList, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testOneItemList, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testListPutIncreasesListSize, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testListBufferIncreaseWillKeepUnpopulatedEntriesEmpty, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testListOfOne, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/list.c", tests, NULL, NULL);

    return ret;
}
