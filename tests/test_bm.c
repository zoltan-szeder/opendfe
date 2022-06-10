#include "bm.h"
#include "test_fixtures.h"
#include "pal.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void testBmReadFile();
void testBmCreateImage_1();
void testBmCreateImage_2();
void testBmCreateImage_3();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testBmReadFile,
        &testBmCreateImage_1,
        &testBmCreateImage_2,
        &testBmCreateImage_3,
    };

    TestFixture fixture = createFixture();

    fixture.name = "bm.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

Palette getSimplePalette() {
    Palette palette;

    ucvec3 white; white.r = 255; white.g = 255; white.b = 255;
    ucvec3 red;   red.r   = 255; red.g   =   0; red.b   =   0;
    ucvec3 green; green.r =   0; green.g = 255; green.b =   0;
    ucvec3 blue;  blue.r  =   0; blue.g  =   0; blue.b  = 255;

    palette.colors[0] = white;      // Top left corner
    palette.colors[15] = red;       // Top right corner
    palette.colors[255-15] = green; // Bottom left corner
    palette.colors[255] = blue;     // Bottom right corner

    return palette;
}

void assertPixel(Image8Bit* img, uint32 i, uint8 r, uint8 g, uint8 b, uint8 a) {
    ucvec4* pixels = (ucvec4*) img->data;
    ucvec4* pixel = pixels + i;
    assert(pixel->r == r);
    assert(pixel->g == g);
    assert(pixel->b == b);
    assert(pixel->a == a);
}

BMHeader getTransparentHeader(int width, int height) {
    BMHeader header;
    header.sizeX = width;
    header.sizeY = height;
    header.dataSize = width * height;
    header.transparent = BM_TRANSPARENT;

    return header;
}

void testBmReadFile(){
    BMFile* bmFile = bmOpenFile("tests/resources/test.bm");

    Palette palette = getSimplePalette();

    bmClose(bmFile);
}

void testBmCreateImage_1() {
    BMHeader header = getTransparentHeader(1, 1);
    unsigned char image[] = {
        15
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, 255, 0, 0, 255);

    img8bDelete(img);
}

void testBmCreateImage_2() {
    BMHeader header = getTransparentHeader(2, 1);
    unsigned char image[] = {
    //  TL   TR
        15, 255
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, 255, 0, 0, 255);
    assertPixel(img, 1, 0, 0, 255, 255);

    img8bDelete(img);
}

void testBmCreateImage_3() {
    BMHeader header = getTransparentHeader(1, 2);
    unsigned char image[] = {
    //  BL   TL
        15, 255
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, 0, 0, 255, 255);
    assertPixel(img, 1, 255, 0, 0, 255);

    img8bDelete(img);
}
