#include "test_fixtures.h"
#include "assertions/memory.h"

#include "odf/sys/optional.h"

#include <string.h>

typedef struct {
    uint8_t c;
    uint32_t n;
} TestStruct;

void tearDown(){
    assertAllMemoryReleased();

    if(!memoryIsEmpty()) {
        memoryDump(false);
    }
}

void testOptional(){
    testCase("testOptional");

    TestStruct ts = {
        .c = 'c',
        .n = 32,
    };

    OptionalPtr* optional = optionalOf(&ts);

    assertFalse(optionalIsEmpty(optional));

    TestStruct* tsPtr = optionalGet(optional);

    assertEquali(tsPtr->c, ts.c);
    assertEquali(tsPtr->n, ts.n);
}

void testEmptyOptional(){
    testCase("testEmptyOptional");

    OptionalPtr* optional = optionalEmpty("testEmptyOptional - This optional is empty");

    assertTrueMsg(optionalIsEmpty(optional), error("Optional is not empty"));

    char* msg = optionalGetMessage(optional);
    char* expectedMessage = "tests/odf/sys/test_optional.c:L42:"
        "testEmptyOptional - This optional is empty";
    assertEquals(expectedMessage, msg, strlen(expectedMessage));

    memoryRelease(msg);
}

void testEmptyOptionalWithParameters(){
    testCase("testEmptyOptionalWithParameters");

    OptionalPtr* optional = optionalEmpty("testEmptyOptionalWithParameters - This optional is \"%s\"", "empty");

    assertTrueMsg(optionalIsEmpty(optional), error("Optional is not empty"));

    char* msg = optionalGetMessage(optional);
    char* expectedMessage = "tests/odf/sys/test_optional.c:L57:"
    "testEmptyOptionalWithParameters - This optional is \"empty\"";
    assertEquals(expectedMessage, msg, strlen(expectedMessage));

    memoryRelease(msg);
}

void testOptionalCanBeDeleted(){
    testCase("testOptionalCanBeDeleted");

    TestStruct ts = {
        .c = 'c',
        .n = 32,
    };

    OptionalPtr* optional = optionalOf(&ts);

    optionalDelete(optional);
}

void testEmptyOptionalCanBeDeleted(){
    testCase("testEmptyOptionalCanBeDeleted");

    OptionalPtr* optional = optionalEmpty("message");

    optionalDelete(optional);
}

void testOptionalOfNullIsAnEmptyOptional(){
    testCase("testOptionalOfNullIsAnEmptyOptional");

    OptionalPtr* optional = optionalOf(NULL);

    assertTrue(optionalIsEmpty(optional));

    char* msg = optionalGetMessage(optional);

    assertTrueMsg(strlen(msg) > 0, error("Optional of NULL doesn't contain any error message"));

    memoryRelease(msg);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testOptional,
        &testEmptyOptional,
        &testEmptyOptionalWithParameters,
        &testOptionalCanBeDeleted,
        &testEmptyOptionalCanBeDeleted,
        &testOptionalOfNullIsAnEmptyOptional,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/optional.c";
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
