#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "test_fixtures.h"

#include "odf/res/bm.h"
#include "odf/res/types/bm_def.h"
#include "odf/res/pal.h"
#include "odf/sys/list.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/log.h"
#include "odf/sys/optional.h"
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

#define TEST_BM_HEADER_2X2 \
    "BM \x1e" "\x02\x00" "\x02\x00" "\x02\x00" "\x02\x00" \
    "\x36" "\x01" "\x00\x00" "\x04\x00\x00\x00" \
    "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
#define TEST_INVERSED_BM_HEADER_2X2 \
    "BM \x1e" "\x01\x00" "\x02\x00" "\x01\x00" "\x02\x00" \
    "\x36" "\x09" "\x00\x00" "\x0\x2\x0\x0" \
    "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
#define TEST_BM_HEADER_MULTIPLE \
    "BM \x1e" "\x1\x0" "\x2\x0" "\x0\x0" "\x0\x0" \
    "\x36" "\x1" "\x0\x0" "\x2\x0\x0\x0" \
    "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
#define TEST_BM_FRAME_RATE "\x19"
#define TEST_BM_2 "\x02"
#define TEST_1ST_SUBHEADER_OFFSET "\x08\x0\x0\x0"
#define TEST_2ND_SUBHEADER_OFFSET "\x28\x0\x0\x0"
#define TEST_BM_SUBHEADER_2X2 \
    "\x2\x0" "\x2\x0" "\x2\x0" "\x2\x0" \
    "\x4\x0\x0\x0" "\x1" "\x0\x0\x0" "\x0\x0\x0" \
    "\x0\x0\x0\x0\x0" "\x36" "\x0\x0\x0"
#define TEST_BM_DATA_1 \
    "\x0\x1\x2\x3"
#define TEST_BM_DATA_2 \
    "\x3\x2\x01\x0"


StringBuffer* sb;
InMemoryFile* file;

int setUp(void** state){
    logSetLevel(WARN);

    sb = stringBufferCreate();
    file = NULL;

    return 0;
}

int tearDown(void** state){
    if(sb) stringBufferDelete(sb);
    if(file) inMemFileDelete(file);

    if(memoryGetAllocations() != 0) {
        memoryDump(false);
        memoryReleaseAll();
        fail_msg("Found remaining memory");
    }

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

    // +-----> y
    // | R B
    // | G A
    // v
    // x
    assertPixel(img, 0, COL_R); // BL
    assertPixel(img, 1, COL_B); // TL
    assertPixel(img, 2, COL_G); // BR
    assertPixel(img, 3, COL_A); // TR

    img8bDelete(img);
}

void testMultipleBM() {
    StringBuffer* sb = stringBufferCreate();
    stringBufferAppendBytes(sb, TEST_BM_HEADER_MULTIPLE,     sizeof(BMHeader));
    stringBufferAppendBytes(sb, TEST_BM_FRAME_RATE,          sizeof(uint8_t));
    stringBufferAppendBytes(sb, TEST_BM_2,                   sizeof(uint8_t));
    stringBufferAppendBytes(sb, TEST_1ST_SUBHEADER_OFFSET,   sizeof(uint32_t));
    stringBufferAppendBytes(sb, TEST_2ND_SUBHEADER_OFFSET,   sizeof(uint32_t));
    stringBufferAppendBytes(sb, TEST_BM_SUBHEADER_2X2,       sizeof(BMSubHeader));
    stringBufferAppendBytes(sb, TEST_BM_DATA_1,            4*sizeof(uint8_t));
    stringBufferAppendBytes(sb, TEST_BM_SUBHEADER_2X2,       sizeof(BMSubHeader));
    stringBufferAppendBytes(sb, TEST_BM_DATA_2,            4*sizeof(uint8_t));

    OptionalOf(InMemoryFile*)* optFile = memFileCreate(stringBufferToString(sb), stringBufferSize(sb));
    assert_false(optionalIsEmpty(optFile));

    InMemoryFile* bmInMemFile = optionalGet(optFile);

    stringBufferDelete(sb);

    OptionalOf(BMFile*)* optBmFile = bmOpenInMemoryFile(bmInMemFile);
    inMemFileDelete(bmInMemFile);

    if(optionalIsEmpty(optBmFile)) {
        fail_msg("Optional BMFile is empty");
    };

    BMFile* bmFile = optionalGet(optBmFile);
    assert_non_null(bmFile->subBMFiles);

    assert_int_equal(25, bmFile->frameRate);

    BMHeader* header = bmFile->header;

    assert_int_equal(1, header->sizeX);
    assert_int_equal(2, header->sizeY);
    assert_int_equal(1, header->logSizeY);
    assert_int_equal(0x36, header->transparent);
    assert_int_equal(0, header->compressed);

    BMSubFile* subFile1 = optionalGet(listGet(bmFile->subBMFiles, 0));
    BMSubHeader* subHeader1 = subFile1->header;
    assert_int_equal(2, subHeader1->sizeX);
    assert_int_equal(2, subHeader1->sizeY);
    assert_int_equal(0x36, subHeader1->transparent);
    assert_int_equal(4, subHeader1->dataSize);


    bmClose(bmFile);
}

void testInverseBM() {
    StringBuffer* sb = stringBufferCreate();
    stringBufferAppendBytes(sb, TEST_INVERSED_BM_HEADER_2X2, sizeof(BMHeader));
    stringBufferAppendBytes(sb, TEST_BM_DATA_1,              4*sizeof(uint8_t));
    uint8_t* data = memoryAllocate(256*512);
    for(size_t i = 0; i < 256*512; i++) data[i] = 0;
    stringBufferAppendBytes(sb, data,                        256*512*sizeof(uint8_t));
    memoryRelease(data);

    InMemoryFile* bmInMemFile = optionalGet(memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));
    stringBufferDelete(sb);

    OptionalOf(BMFile*)* optBmFile = bmOpenInMemoryFile(bmInMemFile);
    inMemFileDelete(bmInMemFile);

    if(optionalIsEmpty(optBmFile)) {
        fail_msg("Optional BMFile is empty");
    };

    BMFile* bmFile = optionalGet(optBmFile);
    BMHeader* header = bmFile->header;

    assert_int_equal(256, header->sizeX);
    assert_int_equal(512, header->sizeY);
    assert_int_equal(9, header->logSizeY);
    assert_int_equal(0x36, header->transparent);
    assert_int_equal(0, header->compressed);

    assert_memory_equal(TEST_BM_DATA_1, bmFile->data, 4*sizeof(uint8_t));


    bmClose(bmFile);
}

OptionalOf(BMFile*)* bmTestRead() {
    file = optionalGet(memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    return bmOpenInMemoryFile(file);
}

void testBmReadInMemFile(){
    // Header
    stringBufferAppendBytes(sb, "BM \x1e", 4);          // 0x00 Magic
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x04 SizeX
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x06 SizeY
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x08 IdemX (ClipSize)
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x0a IdemY (ClipSize)
    stringBufferAppendBytes(sb, "\x36", 1);             // 0x0c Transparent
    stringBufferAppendBytes(sb, "\x00", 1);             // 0x0d LogSizeY
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x0e Compressed
    stringBufferAppendBytes(sb, "\x01\x00\x00\x00", 4); // 0x10 DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x14 Pad1
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x18 Pad2
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x1c Pad3

    stringBufferAppendBytes(sb, "\x01", 1);             // 0x20 Data

    BMFile* bm = optionalGet(bmTestRead());

    assert_memory_equal(bm->header->magic, "BM \x1e", 4);
    assert_int_equal(bm->header->sizeX, 1);
    assert_int_equal(bm->header->sizeY, 1);
    assert_int_equal(bm->header->idemX, 0);
    assert_int_equal(bm->header->idemY, 0);
    assert_int_equal(bm->header->transparent, '\x36');
    assert_int_equal(bm->header->logSizeY, 0);
    assert_int_equal(bm->header->compressed, BM_COMPRESSION_NONE);
    assert_memory_equal(bm->header->pad, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0", 12);
    assert_memory_equal(bm->data, "\x01", 1);

    bmClose(bm);
}

void testBmReadRLE1CompressedFile(){
    // Header
    stringBufferAppendBytes(sb, "BM \x1e", 4);          // 0x00 Magic
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x04 SizeX
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x06 SizeY
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x08 IdemX (ClipSize)
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x0a IdemY (ClipSize)
    stringBufferAppendBytes(sb, "\x36", 1);             // 0x0c Transparent
    stringBufferAppendBytes(sb, "\x00", 1);             // 0x0d LogSizeY
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x0e Compressed
    stringBufferAppendBytes(sb, "\x06\x00\x00\x00", 4); // 0x10 DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x14 Pad1
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x18 Pad2
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x1c Pad3

    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // 0x20 RLE Offset table
    stringBufferAppendBytes(sb, "\x81\x01", 2);         // 0x24 RLE compressed data

    BMFile* bm = optionalGet(bmTestRead());

    assert_memory_equal(bm->header->magic, "BM \x1e", 4);
    assert_int_equal(bm->header->sizeX, 1);
    assert_int_equal(bm->header->sizeY, 1);
    assert_int_equal(bm->header->idemX, 0);
    assert_int_equal(bm->header->idemY, 0);
    assert_int_equal(bm->header->transparent, '\x36');
    assert_int_equal(bm->header->logSizeY, 0);
    assert_int_equal(bm->header->compressed, BM_COMPRESSION_RLE1);
    assert_memory_equal(bm->header->pad, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0", 12);
    assert_memory_equal(bm->data, "\x01", 1);

    bmClose(bm);
}

void testBmReadRLE0CompressedFile(){
    // Header
    stringBufferAppendBytes(sb, "BM \x1e", 4);          // 0x00 Magic
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x04 SizeX
    stringBufferAppendBytes(sb, "\x01\x00", 2);         // 0x06 SizeY
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x08 IdemX (ClipSize)
    stringBufferAppendBytes(sb, "\x00\x00", 2);         // 0x0a IdemY (ClipSize)
    stringBufferAppendBytes(sb, "\x36", 1);             // 0x0c Transparent
    stringBufferAppendBytes(sb, "\x00", 1);             // 0x0d LogSizeY
    stringBufferAppendBytes(sb, "\x02\x00", 2);         // 0x0e Compressed
    stringBufferAppendBytes(sb, "\x06\x00\x00\x00", 4); // 0x10 DataSize
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x14 Pad1
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x18 Pad2
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); // 0x1c Pad3

    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4); // 0x20 RLE Offset table
    stringBufferAppendBytes(sb, "\x81", 1);         // 0x24 RLE compressed data

    BMFile* bm = optionalGet(bmTestRead());

    assert_memory_equal(bm->header->magic, "BM \x1e", 4);
    assert_int_equal(bm->header->sizeX, 1);
    assert_int_equal(bm->header->sizeY, 1);
    assert_int_equal(bm->header->idemX, 0);
    assert_int_equal(bm->header->idemY, 0);
    assert_int_equal(bm->header->transparent, '\x36');
    assert_int_equal(bm->header->logSizeY, 0);
    assert_int_equal(bm->header->compressed, BM_COMPRESSION_RLE0);
    assert_memory_equal(bm->header->pad, "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0", 12);
    assert_memory_equal(bm->data, "\x00", 1);

    bmClose(bm);
}



int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testBmReadFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_1, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_2, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_3, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmCreateImage_4, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testMultipleBM, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testInverseBM, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmReadInMemFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmReadRLE1CompressedFile, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testBmReadRLE0CompressedFile, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/bm.c", tests, NULL, NULL);

    return ret;
}
