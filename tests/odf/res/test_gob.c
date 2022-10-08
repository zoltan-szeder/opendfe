#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"
#include "assertions/memory.h"

#include "odf/res/gob.h"

void tearDown(){
    assertAllMemoryReleased();
}

void testOpenMissingFile(){
    testCase("testOpenMissingFile");
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    List* files = gobListFiles(archive);
    GobFile* file = optionalGet(listGet(files, 0));

    assert(archive != NULL);
    assert(gobCountFiles(archive) == 1);
    assert(strcmp("HELLO.TXT", gobGetFileName(file)) == 0);
    
    gobCloseArchive(archive);
}

void testReadFile() {
    testCase("testReadFile");
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    GobFile* helloFile = gobGetFile(archive, "HELLO.TXT");
    InMemoryFile* hello = gobReadFile(helloFile);

    assert(gobCountFiles(archive) == 1);
    assertEquals("Hello World", hello->content, 12);

    inMemFileDelete(hello);
    gobCloseArchive(archive);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testOpenMissingFile,
        &testReadFile,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/res/gob.c";
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
