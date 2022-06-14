#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "test_fixtures.h"

#include "drivers/bm.h"
#include "drivers/pal.h"

#define PX_A 0
#define PX_R 15
#define PX_G 240
#define PX_B 255

#define COL_A   0,   0,   0,   0
#define COL_R 255,   0,   0, 255
#define COL_G   0, 255,   0, 255
#define COL_B   0,   0, 255, 255

void testBmReadFile();
void testBmCreateImage_1();
void testBmCreateImage_2();
void testBmCreateImage_3();
void testBmCreateImage_4();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testBmReadFile,
        &testBmCreateImage_1,
        &testBmCreateImage_2,
        &testBmCreateImage_3,
        &testBmCreateImage_4,
    };

    TestFixture fixture = createFixture();

    fixture.name = "bm.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    return runTests(&fixture);
}

Palette getSimplePalette() {
    Palette palette;

    ucvec3 alpha;  alpha.r  =   0; alpha.g  =   0; alpha.b  =   0;
    ucvec3 red;    red.r    = 255; red.g    =   0; red.b    =   0;
    ucvec3 green;  green.r  =   0; green.g  = 255; green.b  =   0;
    ucvec3 blue;   blue.r   =   0; blue.g   =   0; blue.b   = 255;

    palette.colors[PX_A] = alpha;
    palette.colors[PX_R] = red;
    palette.colors[PX_G] = green;
    palette.colors[PX_B] = blue;

    return palette;
}

bool isPixel(ucvec4* pixel, uint8 r, uint8 g, uint8 b, uint8 a) {
    return pixel->r == r
        && pixel->g == g
        && pixel->b == b
        && pixel->a == a;
}

void assertPixel(Image8Bit* img, uint32 i, uint8 r, uint8 g, uint8 b, uint8 a) {
    ucvec4* pixels = (ucvec4*) img->data;
    ucvec4* pixel = pixels + i;
    assertTrueMsg(isPixel(pixel, r, g, b, a), error(
        "Color (%d,%d,%d,%d) does not match expected (%d,%d,%d,%d)",
        pixel->r, pixel->g, pixel->b, pixel->a,
        r, g, b, a));
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
    testCase("Test .BM file parsing");
    BMFile* bmFile = bmOpenFile("tests/resources/test.bm");

    Palette palette = getSimplePalette();

    bmClose(bmFile);
}

void testBmCreateImage_1() {
    testCase("Test 1x1 image parse");
    BMHeader header = getTransparentHeader(1, 1);
    unsigned char image[] = {
        PX_R
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, COL_R);

    img8bDelete(img);
}

void testBmCreateImage_2() {
    testCase("Test 2x1 image parse");
    BMHeader header = getTransparentHeader(2, 1);
    unsigned char image[] = {
    //    TL    TR
        PX_R, PX_G
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, COL_R);
    assertPixel(img, 1, COL_G);

    img8bDelete(img);
}

void testBmCreateImage_3() {
    testCase("Test 1x2 image parse");
    BMHeader header = getTransparentHeader(1, 2);
    unsigned char image[] = {
    //    BL    TL
        PX_R, PX_G
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    assertPixel(img, 0, COL_R);
    assertPixel(img, 1, COL_G);

    img8bDelete(img);
}


void testBmCreateImage_4() {
    testCase("Test 2x2 image parse");
    BMHeader header = getTransparentHeader(2, 2);
    // +-----> y
    // | R B
    // | G A
    // v
    // x
    unsigned char image[] = {
        PX_R, PX_B,
        PX_G, PX_A,
    };

    BMFile bm;
    bm.header = &header;
    bm.data = image;

    Palette pal = getSimplePalette();

    Image8Bit* img = bmCreateImage(&bm, &pal);

    // +-----> x
    // | R G
    // | B A
    // v
    // y
    assertPixel(img, 0, COL_R); // BL
    assertPixel(img, 1, COL_G); // BR
    assertPixel(img, 2, COL_B); // TL
    assertPixel(img, 3, COL_A); // TR

    img8bDelete(img);
}
