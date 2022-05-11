#include "../headers/bm.h"
#include "headers/test_fixtures.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void testBmReadFile();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testBmReadFile
    };

    TestFixture fixture = createFixture();

    fixture.name = "bm.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testBmReadFile(){
    BMFile* bmFile = bmOpenFile("/home/fruzsi/GOG Games/STAR WARS Dark Forces/scripts/TEXTURES/ZPYFILL.BM");

    bmPrintFile(bmFile);

    bmCloseFile(bmFile);
}
