#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"

#include "types.h"
#include "system/memory.h"
#include "inmemoryfile.h"

void testRead1ByteFile();
void testSeek();
void testInMemFileReadAll();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        //&testRead1ByteFile,
        &testSeek,
        &testInMemFileReadAll,
    };

    TestFixture fixture = createFixture();

    fixture.name = "inmemoryfile.c";
    fixture.before = &setToLittleEndian;
    fixture.tests = testFunctions;
    fixture.after = &setToOriginalEndianness;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

OptionalPtr* createTestInMemFile(char* content) {
    size_t length = strlen(content)+1;
    OptionalPtr* optional = memoryAllocate(length*sizeof(char));
    if(optionalIsEmpty(optional)) return optional;

    char* contentBytes = optionalGet(optional);
    memcpy(contentBytes, content, length);

    return  memFileCreate(contentBytes, length);
}

void testRead1ByteFile() {
    testCase("Test file will not read after EOF");
    char* content = "abcdefg";
    OptionalPtr* optionalFile = createTestInMemFile(content);
    if(optionalIsEmpty(optionalFile)) {
        fail(optionalGetMessage(optionalFile));
        return;
    }

    InMemoryFile* file = optionalGet(optionalFile);

    assertEquali(0, memFileSeek(file, -1, SEEK_END));
    OptionalUInt64* optUInt64 = inMemFileReadLittleEndianUInt64(file);
    assertEquali(0, memFileSeek(file, -1, SEEK_END));
    OptionalUInt32* optUInt32 = inMemFileReadLittleEndianUInt32(file);
    assertEquali(0, memFileSeek(file, -1, SEEK_END));
    OptionalUInt16* optUInt16 = inMemFileReadLittleEndianUInt16(file);
    assertEquali(0, memFileSeek(file, -1, SEEK_END));
    OptionalUInt8* optUInt8Pass = inMemFileReadLittleEndianUInt8(file);
    assertEquali(0, memFileSeek(file, 0, SEEK_END));
    OptionalUInt8* optUInt8Fail = inMemFileReadLittleEndianUInt8(file);

    assertTrue(optionalIsEmpty(optUInt64));
    memoryRelease(optionalGetMessage(optUInt64));
    assertTrue(optionalIsEmpty(optUInt32));
    memoryRelease(optionalGetMessage(optUInt32));
    assertTrue(optionalIsEmpty(optUInt16));
    memoryRelease(optionalGetMessage(optUInt16));
    assertFalse(optionalIsEmpty(optUInt8Pass));
    assertEquali('\x00', optionalGetUInt8(optUInt8Pass));
    assertTrue(optionalIsEmpty(optUInt8Fail));
    memoryRelease(optionalGetMessage(optUInt8Fail));

    inMemFileDelete(file);
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

void testSeek() {
    testCase("Test seek");
    char* content = "0123456789";
    OptionalPtr* optionalFile = createTestInMemFile(content);
    if(optionalIsEmpty(optionalFile)) {
        fail(optionalGetMessage(optionalFile));
        return;
    }

    InMemoryFile* file = optionalGet(optionalFile);

    assertEquali(1, memFileSeek(file, 12, SEEK_SET));
    assertEquali(0, memFileSeek(file, 11, SEEK_SET));
    assertOptionalIsEmpty(inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 10, SEEK_SET));
    assertOptionalUInt8('\x00', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 9, SEEK_SET));
    assertOptionalUInt8('9', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 1, SEEK_SET));
    assertOptionalUInt8('1', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 0, SEEK_SET));
    assertOptionalUInt8('0', inMemFileReadLittleEndianUInt8(file));
    assertEquali(1, memFileSeek(file, -1, SEEK_SET));

    assertEquali(1, memFileSeek(file, 1, SEEK_END));
    assertEquali(0, memFileSeek(file, 0, SEEK_END));
    assertOptionalIsEmpty(inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, -1, SEEK_END));
    assertOptionalUInt8('\x00', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, -2, SEEK_END));
    assertOptionalUInt8('9', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, -10, SEEK_END));
    assertOptionalUInt8('1', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, -11, SEEK_END));
    assertOptionalUInt8('0', inMemFileReadLittleEndianUInt8(file));
    assertEquali(1, memFileSeek(file, -12, SEEK_END));


    assertEquali(0, memFileSeek(file, 0, SEEK_SET));

    assertEquali(1, memFileSeek(file, -1, SEEK_CUR));
    assertEquali(0, memFileSeek(file, 0, SEEK_CUR));
    assertOptionalUInt8('0', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 1, SEEK_CUR));
    assertOptionalUInt8('2', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, -1, SEEK_CUR));
    assertOptionalUInt8('2', inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 8, SEEK_CUR));
    assertOptionalIsEmpty(inMemFileReadLittleEndianUInt8(file));
    assertEquali(0, memFileSeek(file, 0, SEEK_CUR));
    assertEquali(1, memFileSeek(file, 1, SEEK_CUR));

    inMemFileDelete(file);
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
