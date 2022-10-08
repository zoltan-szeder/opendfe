#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"
#include "assertions/memory.h"

#include "odf/sys/file.h"


void tearDown(){
    assertAllMemoryReleased();
}

void testOpenMissingFile(){
    OptionalPtr* optional = fileOpen("missing/nonExistent.file", "rb");
    assertTrueMsg(optionalIsEmpty(optional), error("File should be missing, but is present"));
    char* msg = optionalGetMessage(optional);
    assertTrue(msg != NULL);
    memoryRelease(msg);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testOpenMissingFile,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/file.c";
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
