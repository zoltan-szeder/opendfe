#include "../headers/bm.h"
#include "headers/test_fixtures.h"
#include "../headers/pal.h"
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
    BMFile* bmFile = bmOpenFile("tests/resources/test.bm");

    Palette palette;

    Color white; white.r = 255; white.g = 255; white.b = 255;
    Color red;   red.r   = 255; red.g   =   0; red.b   =   0;
    Color green; green.r =   0; green.g = 255; green.b =   0;
    Color blue;  blue.r  =   0; blue.g  =   0; blue.b  = 255;

    palette.colors[0] = white;      // Top left corner
    palette.colors[15] = red;       // Top right corner
    palette.colors[255-15] = green; // Bottom left corner
    palette.colors[255] = blue;     // Bottom right corner

    uint32 image = bmGlBindImageTexture(bmFile, &palette);
    bmCloseFile(bmFile);
}
