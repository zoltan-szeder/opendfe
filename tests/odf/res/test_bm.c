#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "test_fixtures.h"

#include "odf/res/bm.h"
#include "odf/res/types/bm_def.h"
#include "odf/res/pal.h"
#include "odf/res/types/pal_def.h"
#include "system/test_memory.h"

#define PX_A 0
#define PX_R 15
#define PX_G 240
#define PX_B 255

#define COL_A   0,   0,   0,   0
#define COL_R 255,   0,   0, 255
#define COL_G   0, 255,   0, 255
#define COL_B   0,   0, 255, 255


int tearDown(void** state){
    assert_int_equal(0, memoryGetAllocations());

    return 0;
}


Palette getSimplePalette() {
    Palette palette;

    ucvec3 alpha;  alpha.r  =   0; alpha.g  =   0; alpha.b  =   0;
    ucvec3 red;    red.r    =  63; red.g    =   0; red.b    =   0;
    ucvec3 green;  green.r  =   0; green.g  =  63; green.b  =   0;
    ucvec3 blue;   blue.r   =   0; blue.g   =   0; blue.b   =  63;

    palette.colors[PX_A] = alpha;
    palette.colors[PX_R] = red;
    palette.colors[PX_G] = green;
    palette.colors[PX_B] = blue;

    return palette;
}

bool isPixel(ucvec4* pixel, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return pixel->r == r
        && pixel->g == g
        && pixel->b == b
        && pixel->a == a;
}

void assertPixel(Image8Bit* img, uint32_t i, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    ucvec4* pixels = (ucvec4*) img->data;
    ucvec4* pixel = pixels + i;
    assert_true(isPixel(pixel, r, g, b, a));
}

BMHeader getTransparentHeader(int width, int height) {
    BMHeader header;
    header.sizeX = width;
    header.sizeY = height;
    header.dataSize = width * height;
    header.transparent = BM_TRANSPARENT;
    header.compressed = BM_COMPRESSION_NONE;

    return header;
}

void testBmReadFile(){
    BMFile* bmFile = bmOpenFile("tests/resources/test.bm");

    bmClose(bmFile);
}

void testBmCreateImage_1() {
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
    BMHeader header = getTransparentHeader(2, 2);
    // +-----> y
    // | R B
    // | G A
    // v
    // x
    uint8_t image[] = {
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

void testRLE0EncodedBmCreateImage() {
    BMHeader header = getTransparentHeader(2, 2);
    header.compressed = BM_COMPRESSION_RLE0;
    // +-----> y
    // | R B
    // | G A
    // v
    // x
    header.dataSize = 7;
    unsigned char image[] = {
        '\x01', PX_R, '\x01', PX_B,
        '\x01', PX_G, '\x81',
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

void testRLE1EncodedBmCreateImage() {
    BMHeader header = getTransparentHeader(2, 2);
    header.compressed = BM_COMPRESSION_RLE1;
    // +-----> y
    // | R B
    // | G A
    // v
    // x
    header.dataSize = 8;
    unsigned char image[] = {
        '\x01', PX_R, '\x81', PX_B,
        '\x01', PX_G, '\x81', PX_A
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


int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testBmReadFile, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_1, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_2, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_3, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_4, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testRLE0EncodedBmCreateImage, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testRLE1EncodedBmCreateImage, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/bm.c", tests, NULL, NULL);

    return ret;
}
