#include "test_fixtures.h"
#include "assertions/memory.h"

#include "odf/sys/list.h"

#include <stdint.h>
#include "assertions/optional.h"

List* list;

void tearDown() {
    listDelete(list);
    assertAllMemoryReleased();
}

typedef struct {
    uint32_t value;
} MyStruct;

void testEmptyList() {
    testCase("testEmptyList");

    list = listCreate(0);

    assertEquali(0, listSize(list));
    assertOptionalIsEmpty(listGet(list, 0));
}

void testAppendToEmptyList() {
    testCase("testAppendToEmptyList");

    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listAppend(list, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));

    assertEquali(ms.value, msPtr->value);
}

void testAppendToList() {
    testCase("testAppendToList");

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

    assertEquali(ms1.value, ms1Ptr->value);
    assertEquali(ms2.value, ms2Ptr->value);
}

void testOneMissingItemList() {
    testCase("testOneMissingItemList");

    list = listCreate(1);

    assertOptionalIsEmpty(listGet(list, 0));
    assertOptionalIsEmpty(listGet(list, 1));

    assertEquali(0, listSize(list));
}

void testOneItemList() {
    testCase("testOneItemList");
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(1);
    listPut(list, 0, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assertEquali(ms.value, msPtr->value);
}

void testListPutIncreasesListSize() {
    testCase("testListPutIncreasesListSize");
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listPut(list, 0, &ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assertEquali(ms.value, msPtr->value);
}
void testListBufferIncreaseWillKeepUnpopulatedEntriesEmpty() {
    testCase("testListBufferIncreaseWillKeepUnpopulatedEntriesEmpty");
    MyStruct ms = {
        .value = 3
    };

    list = listCreate(0);
    listPut(list, 10, &ms);

    assertOptionalIsEmpty(listGet(list, 0));

    MyStruct* msPtr = optionalGet(listGet(list, 10));
    assertEquali(ms.value, msPtr->value);
}

void testListOfOne() {
    testCase("testListOfOne");

    MyStruct ms = {
        .value = 3
    };

    list = listOf(&ms);

    MyStruct* msPtr = optionalGet(listGet(list, 0));
    assertEquali(3, msPtr->value);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testEmptyList,
        &testAppendToEmptyList,
        &testAppendToList,
        &testOneMissingItemList,
        &testOneItemList,
        &testListPutIncreasesListSize,
        &testListBufferIncreaseWillKeepUnpopulatedEntriesEmpty,
        &testListOfOne,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/list.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
