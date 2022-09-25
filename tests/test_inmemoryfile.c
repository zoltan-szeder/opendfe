#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"

#include "types.h"
#include "system/memory.h"
#include "inmemoryfile.h"
#include "assertions/memory.h"

void tearDown(){
    assertAllMemoryReleased();
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
        fail(error("Optional is not empty"));
    }
}
void assertOptionalUInt8(uint8 value,void* optional){
    if(optionalIsEmpty(optional)) {
        char* msg = optionalGetMessage(optional);
        fail(error("%s", msg));
        memoryRelease(msg);
    } else {
        assertEquali(value, optionalGetUInt8(optional));
    }
}
void assertOptionalBytes(char* expected,void* optional){
    if(optionalIsEmpty(optional)) {
        char* msg = optionalGetMessage(optional);
        fail(error("%s", msg));
        memoryRelease(msg);
    } else {
        void* actual = optionalGet(optional);
        assertEquals(expected, actual, strlen(expected));
        memoryRelease(actual);
    }
}


void testInMemFileReadAll(){
    testCase("testInMemFileReadAll");
    char* content = "abcdefghij";

    OptionalPtr* optionalFile = createTestInMemFile(content);
    if(optionalIsEmpty(optionalFile)) {
        fail(optionalGetMessage(optionalFile));
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
    void (*testFunctions[])() = {
        &testInMemFileReadAll,
    };

    TestFixture fixture = createFixture();

    fixture.name = "inmemoryfile.c";
    fixture.before = &setToLittleEndian;
    fixture.tests = testFunctions;
    fixture.after = &setToOriginalEndianness;
    fixture.afterEach = &tearDown;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
