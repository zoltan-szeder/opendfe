#include "test_fixtures.h"
#include "assertions/memory.h"
#include "assertions/optional.h"

#include <string.h>
#include "odf/sys/strings.h"


void assertStringList(List* strList, size_t expectedListSize, ...) {
    assertEquali(expectedListSize, listSize(strList));

    va_list args;
    va_start(args, expectedListSize);

    for(size_t i = 0; i < expectedListSize; i++) {
        char* expected = va_arg(args, char*);
        size_t expectedSize = strlen(expected);


        char* actual = optionalGet(listGet(strList, i));
        size_t actualSize = strlen(actual);

        assertEquali(expectedSize, actualSize);
        assertEquals(expected, actual, expectedSize);
        memoryRelease(actual);
    }

    va_end(args);

    listDelete(strList);
}



void testReadLines() {
    testCase("testReadLines");

    assertStringList(strSplitByLineBreak(""), 1, "");

    assertStringList(strSplitByLineBreak("\n"), 2, "", "");
    assertStringList(strSplitByLineBreak("\r"), 2, "", "");
    assertStringList(strSplitByLineBreak("\r\n"), 2, "", "");

    assertStringList(strSplitByLineBreak("ab\r"), 2, "ab", "");
    assertStringList(strSplitByLineBreak("\r\nab"), 2, "", "ab");
    assertStringList(strSplitByLineBreak("abc\ndef"), 2, "abc", "def");

    assertStringList(strSplitByLineBreak("abcd\nef"), 2, "abcd", "ef");
    assertStringList(strSplitByLineBreak("abcd\ref"), 2, "abcd", "ef");
    assertStringList(strSplitByLineBreak("abcd\r\nef"), 2, "abcd", "ef");

    assertStringList(strSplitByLineBreak("abcd\ref\rghi"), 3, "abcd", "ef", "ghi");
}


int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testReadLines,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/strings.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

