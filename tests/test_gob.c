#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"

#include "drivers/gob.h"

void testOpenMissingFile();
void testReadFile();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testOpenMissingFile,
        &testReadFile,
    };

    TestFixture fixture = createFixture();

    fixture.name = "gob.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testOpenMissingFile(){
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    List* files = gobListFiles(archive);
    GobFile* file = optionalGet(listGet(files, 0));

    assert(archive != NULL);
    assert(gobCountFiles(archive) == 1);
    assert(strcmp("HELLO.TXT", gobGetFileName(file)) == 0);
    
    gobCloseArchive(archive);
}
void testReadFile() {
    GobArchive* archive = optionalGet(gobOpenArchive("tests/resources/test.gob"));
    GobFile* helloFile = gobGetFile(archive, "HELLO.TXT");
    InMemoryFile* hello = gobReadFile(helloFile);

    assert(gobCountFiles(archive) == 1);
    assertEquals("Hello World", hello->content, 12);

    inMemFileDelete(hello);
    gobCloseArchive(archive);
}
