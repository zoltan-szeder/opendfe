#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"

#include "odf/sys/types.h"
#include "odf/sys/memory.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "system/test_memory.h"

int tearDownClass(void** state){
    setToOriginalEndianness();

    return 0;
}

int tearDown(void** state){
    assert_true(memoryGetAllocations() == 0);

    return 0;
}

OptionalPtr* createTestInMemFile(char* content) {
    size_t length = strlen(content)+1;
    char* contentBytes = memoryAllocate(length*sizeof(char));
    memcpy(contentBytes, content, length);

    return  memFileCreate(contentBytes, length);
}


void assertOptionalIsEmpty(void* optional){
    if(optionalIsEmpty(optional)) {
        memoryRelease(optionalGetMessage(optional));
    } else {
        fail();
    }
}

void assertOptionalBytes(char* expected,void* optional){
    if(optionalIsEmpty(optional)) {
        char* msg = optionalGetMessage(optional);
        fail();
        memoryRelease(msg);
    } else {
        void* actual = optionalGet(optional);
        assert_memory_equal(expected, actual, strlen(expected));
        memoryRelease(actual);
    }
}


void testInMemFileReadAll(){
    char* content = "abcdefghij";

    OptionalPtr* optionalFile = createTestInMemFile(content);
    if(optionalIsEmpty(optionalFile)) {
        fail();
        return;
    }

    InMemoryFile* file = optionalGet(optionalFile);

    assertOptionalIsEmpty(inMemFileReadStruct(file, "%c12"));
    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("a", inMemFileReadStruct(file, "%c1"));
    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("ba", inMemFileReadStruct(file, "%b2"));
    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("bac", inMemFileReadStruct(file, "%b2%c1"));
    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("acb", inMemFileReadStruct(file, "%c1%b2"));

    setToBigEndian();

    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("abc", inMemFileReadStruct(file, "%c1%b2"));
    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("cba", inMemFileReadStruct(file, "%l3"));

    setToLittleEndian();

    memFileSeek(file, 0, SEEK_SET);
    assertOptionalBytes("abcde", inMemFileReadStruct(file, "%c1%l4"));

    inMemFileDelete(file);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testInMemFileReadAll, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/inmemoryfile.c", tests, NULL, tearDownClass);

    return ret;
}
