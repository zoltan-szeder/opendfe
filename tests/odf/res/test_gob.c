#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"
#include "assertions/memory.h"

#include "odf/res/gob.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

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
    GobFile* helloFile = optionalGet(gobGetFile(archive, "HELLO.TXT"));
    InMemoryFile* hello = optionalGet(gobReadFile(helloFile));

    OptionalPtr* optContent = inMemFileRead(hello, inMemFileSize(hello));
    if(optionalIsEmpty(optContent)) {
        fail(error("Could not find of test.gob/HELLO.TXT"));
        return;
    }

    char* content = optionalGet(optContent);

    assert(gobCountFiles(archive) == 1);
    assertEquals("Hello World", content, 12);

    memoryRelease(content);
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
