#include "test_fixtures.h"
#include "assertions/memory.h"
#include "system/stringbuffer.h"

StringBuffer* sb;
char* str;

void setUp(){
    sb = stringBufferCreate(100);
}

void tearDown() {
    memoryRelease(str);
    stringBufferDelete(sb);

    assertAllMemoryReleased();
    memoryReleaseAll();
}

void testStringBuffer(){
    testCase("testStringBuffer");

    str = stringBufferToString(sb);
    assertEquals("", str, 0);
}

void testStringBufferWithOneChar(){
    testCase("testStringBufferWithOneChar");

    stringBufferAppend(sb, "a");

    assertEquali(1, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("a", str, 1);
}

void testStringBufferWithDifferentChar(){
    testCase("testStringBufferWithDifferentChar");

    stringBufferAppend(sb, "b");

    assertEquali(1, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("b", str, 1);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testStringBuffer,
        &testStringBufferWithOneChar,
        &testStringBufferWithDifferentChar,
    };

    TestFixture fixture = createFixture();

    fixture.name = "system/stringbuffer.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    return runTests(&fixture);
}
