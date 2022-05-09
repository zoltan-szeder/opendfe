#include "../headers/gob.h"
#include "headers/test_fixtures.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void testSuccess();
void testArchiveCanBeOpenedAndClosed();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testSuccess,
        &testArchiveCanBeOpenedAndClosed
    };

    TestFixture fixture = createFixture();

    fixture.name = "gob.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testArchiveCanBeOpenedAndClosed(){
    GobArchive* archive = gobOpenArchive("tests/resources/test.gob");

    assert(archive != NULL);
    
    gobCloseArchive(archive);
}

void testSuccess() {
    GobArchive* archive = gobOpenArchive("tests/resources/test.gob");
    GobFile* helloFile = gobGetFile(archive, "HELLO.TXT");
    InMemoryFile* hello = gobReadFile(helloFile);

    assert(gobCountFiles(archive) == 1);
    assert(strcmp("Hello World", hello->content) == 0);

    gobCloseFile(hello);
    gobCloseArchive(archive);
}
