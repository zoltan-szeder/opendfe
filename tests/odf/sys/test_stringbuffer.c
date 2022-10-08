#include "test_fixtures.h"
#include "assertions/memory.h"
#include "odf/sys/stringbuffer.h"
#include "assertions/string.h"

StringBuffer* sb;
char* str;

void setUp(){
    sb = stringBufferCreate();
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

void testStringBufferWithSpecialChars(){
    testCase("testStringBufferWithSpecialChars");

    stringBufferAppendBytes(sb, "\x00\x01\x02\x03\x04", 5);
    stringBufferAppendBytes(sb, "\x05\x06\x07\x08\x09", 5);

    assertEquali(10, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", str, 10);
}

void testStringBufferWithDifferentChar(){
    testCase("testStringBufferWithDifferentChar");

    stringBufferAppend(sb, "b");

    assertEquali(1, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("b", str, 1);
}

void testStringBufferWithString(){
    testCase("testStringBufferWithString");

    stringBufferAppend(sb, "ab");

    assertEquali(2, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("ab", str, 2);
}


void testStringBufferMultipleAppend(){
    testCase("testStringBufferMultipleAppend");

    stringBufferAppend(sb, "a");
    stringBufferAppend(sb, "b");

    assertEquali(2, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("ab", str, 2);
}

void testStringBufferIsWillNotOverflow(){
    testCase("testStringBufferIsWillNotOverflow");

    stringBufferAppend(sb, "abcdefghijklmnopqrstuvwxyz");

    assertEquali(26, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("abcdefghijklmnopqrstuvwxyz", str, 26);
}

void testStringBufferIsWillNotOverflowWritingMultipleTimes(){
    testCase("testStringBufferIsWillNotOverflowWritingMultipleTimes");

    stringBufferAppend(sb, "abcdefghij");
    stringBufferAppend(sb, "klmnopqrstuvwxyz");

    assertEquali(26, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("abcdefghijklmnopqrstuvwxyz", str, 26);
    assertNullTerminatedString(str, 26);
}
void testStringBufferWriteFull(){
    testCase("testStringBufferWriteFull");

    stringBufferAppend(sb, "abcde");
    stringBufferAppend(sb, "fghij");

    assertEquali(10, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assertEquals("abcdefghij", str, 10);
    assertNullTerminatedString(str, 10);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testStringBuffer,
        &testStringBufferWithOneChar,
        &testStringBufferWithDifferentChar,
        &testStringBufferWithSpecialChars,
        &testStringBufferWithString,
        &testStringBufferMultipleAppend,
        &testStringBufferIsWillNotOverflow,
        &testStringBufferIsWillNotOverflowWritingMultipleTimes,
        &testStringBufferWriteFull,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/stringbuffer.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    return runTests(&fixture);
}
