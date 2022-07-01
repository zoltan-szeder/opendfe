#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"

#include "file.h"

void testOpenMissingFile();
void testReadFile();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testOpenMissingFile,
        &testReadFile,
    };

    TestFixture fixture = createFixture();

    fixture.name = "file.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testOpenMissingFile(){
    OptionalPtr* optional = fileOpen("missing/nonExistent.file", "rb");
    assertTrueMsg(optionalIsEmpty(optional), error("File should be missing, but is present"));
    char* msg = optionalGetMessage(optional);
    assertTrue(msg != NULL);
    free(msg);
}
void testReadFile() {
}
