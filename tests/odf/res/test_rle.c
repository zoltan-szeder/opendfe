#include "test_fixtures.h"

#include "odf/res/rle.h"

#include <stddef.h>
#include <stdlib.h>

#include "system/test_memory.h"
#include "odf/sys/stringbuffer.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/types.h"
#include "odf/sys/log.h"

StringBuffer* sb;
InMemoryFile* file;

int setUp(void** state){
    memoryReleaseAll();
    logSetLevel(TRACE);
    sb = stringBufferCreate();
    file = NULL;

    return 0;
}

int tearDown(void** state){
    stringBufferDelete(sb);
    if(file) inMemFileDelete(file);

    size_t allocations = memoryGetAllocations();
    if(allocations != 0)
        memoryDump(false);

    assert_int_equal(0, allocations);
    return 0;
}

void testLRE0Decompress_1(){
    int w = 1;
    int h = 1;
    char* lre = "\x01\x01";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assert_int_equal(image[0], lre[1]);
    memoryRelease(image);
}

void testLRE0Decompress_2(){
    int w = 1;
    int h = 1;
    char* lre = "\x01\x02";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assert_int_equal(image[0], lre[1]);
    memoryRelease(image);
}

void testLRE0Decompress_3(){
    int w = 2;
    int h = 1;
    char* lre = "\x02\x01\x02";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assert_int_equal(image[0], lre[1]);
    assert_int_equal(image[1], lre[2]);

    memoryRelease(image);
}

void testLRE0Decompress_4(){
    int w = 3;
    int h = 1;
    char* lre = "\x02\x01\x02\x81";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assert_int_equal(image[0], lre[1]);
    assert_int_equal(image[1], lre[2]);
    assert_int_equal(image[2], '\x00');

    memoryRelease(image);
}


void testLRE0Decompress_5(){
    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x82";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assert_int_equal(image[0], lre[1]);
    assert_int_equal(image[1], lre[2]);
    assert_int_equal(image[2], '\x00');
    assert_int_equal(image[3], '\x00');

    memoryRelease(image);
}


void testLRE0Decompress_6(){
    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x02\x03\x04";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 6, w, h);

    assert_int_equal(image[0], '\x01');
    assert_int_equal(image[1], '\x02');
    assert_int_equal(image[2], '\x03');
    assert_int_equal(image[3], '\x04');

    memoryRelease(image);
}

void testLRE0Decompress_7(){
    int w = 2;
    int h = 2;
    char* lre = "\x83\x01\x04";

    uint8_t* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assert_int_equal(image[0], '\x00');
    assert_int_equal(image[1], '\x00');
    assert_int_equal(image[2], '\x00');
    assert_int_equal(image[3], '\x04');

    memoryRelease(image);
}


void testLRE1Decompress_1(){
    int w = 1;
    int h = 1;
    char* lre = "\x01\x01";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 2, w, h);

    assert_int_equal(image[0], lre[1]);
    memoryRelease(image);
}

void testLRE1Decompress_2(){
    int w = 1;
    int h = 1;
    char* lre = "\x01\x02";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 2, w, h);

    assert_int_equal(image[0], lre[1]);
    memoryRelease(image);
}

void testLRE1Decompress_3(){
    int w = 2;
    int h = 1;
    char* lre = "\x02\x01\x02";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 3, w, h);

    assert_int_equal(image[0], lre[1]);
    assert_int_equal(image[1], lre[2]);

    memoryRelease(image);
}

void testLRE1Decompress_4(){
    int w = 3;
    int h = 1;
    char* lre = "\x02\x01\x02\x81\x03";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 5, w, h);

    assert_int_equal(image[0], '\x01');
    assert_int_equal(image[1], '\x02');
    assert_int_equal(image[2], '\x03');

    memoryRelease(image);
}


void testLRE1Decompress_5(){
    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x82\x03";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 5, w, h);

    assert_int_equal(image[0], '\x01');
    assert_int_equal(image[1], '\x02');
    assert_int_equal(image[2], '\x03');
    assert_int_equal(image[3], '\x03');

    memoryRelease(image);
}


void testLRE1Decompress_6(){
    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x02\x03\x04";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 6, w, h);

    assert_int_equal(image[0], '\x01');
    assert_int_equal(image[1], '\x02');
    assert_int_equal(image[2], '\x03');
    assert_int_equal(image[3], '\x04');

    memoryRelease(image);
}

void testLRE1Decompress_7(){
    int w = 2;
    int h = 2;
    char* lre = "\x83\x01\x01\x04";

    uint8_t* image = rle1Decompress((unsigned char*) lre, 4, w, h);

    assert_int_equal(image[0], '\x01');
    assert_int_equal(image[1], '\x01');
    assert_int_equal(image[2], '\x01');
    assert_int_equal(image[3], '\x04');

    memoryRelease(image);
}

void testLREFileRead_1(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 1
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x01\x02", 2);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x02", data, 1);

    memoryRelease(data);
}

void testLREFileRead_2(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 1
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x01\x03", 2);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x03", data, 1);

    memoryRelease(data);
}

void testLREFileRead_3(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 3,
        .offsetCount = 1,
        .sequenceSize = 1
    };
    stringBufferAppendBytes(sb, "\x00\x00\x00", 3);
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x01\x03", 2);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));
    memFileSeek(file, 3, SEEK_SET);

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x03", data, 1);

    memoryRelease(data);
}

void testLREFileRead_4(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x02\x03\x04", 3);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x03\x04", data, 2);

    memoryRelease(data);
}

void testLREFileRead_5(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x08\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x02\x03\x04", 3);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_6(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x03\x03\x04\x05", 4);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_7(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 2,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x08\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x0b\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x02\x01\x02", 3);
    stringBufferAppendBytes(sb, "\x02\x03\x04", 3);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x01\x02\x03\x04", data, 4);

    memoryRelease(data);
}

void testLREFileRead_8(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 2,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x08\x00", 2);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_9(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x01\x01\x01\x02", 4);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x01\x02", data, 2);

    memoryRelease(data);
}

void testLREFileRead_10(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x82", 1);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x00\x00", data, 2);

    memoryRelease(data);
}

void testLREFileRead_11(){
    RLEConfig config = {
        .type = RLE1,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x82\x01", 2);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x01\x01", data, 2);

    memoryRelease(data);
}

void testLREFileRead_12(){
    RLEConfig config = {
        .type = RLE1,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x82", 1);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_13(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x02", 1);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_14(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 8,
        .offsetCount = 4,
        .sequenceSize = 4
    };
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); //0x00
    stringBufferAppendBytes(sb, "\x00\x00\x00\x00", 4); //0x04
    stringBufferAppendBytes(sb, "\x10\x00\x00\x00", 4); //0x08
    stringBufferAppendBytes(sb, "\x14\x00\x00\x00", 4); //0x0c
    stringBufferAppendBytes(sb, "\x1c\x00\x00\x00", 4); //0x10
    stringBufferAppendBytes(sb, "\x17\x00\x00\x00", 4); //0x14
    stringBufferAppendBytes(sb, "\x82\x02\x03\x04", 4); //0x18
    stringBufferAppendBytes(sb, "\x01\x05\x83\x04", 4); //0x1c
    stringBufferAppendBytes(sb, "\x0d\x0e\x0f\x10", 4); //0x20
    stringBufferAppendBytes(sb, "\x84", 1);             //0x24

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    memFileSeek(file, 8, SEEK_SET);

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x00\x00\x03\x04", data, 4);
    assert_memory_equal("\x05\x00\x00\x00", data+4, 4);
    assert_memory_equal("\x00\x00\x00\x00", data+8, 4);
    assert_memory_equal("\x0d\x0e\x0f\x10", data+12, 4);

    memoryRelease(data);
}

void testLREFileRead_15(){
    RLEConfig config = {
        .type = RLE0,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x01\x00\x02\x01\x02", 5);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    OptionalPtr* optional = rleReadFile(file, &config);
    assert_true(optionalIsEmpty(optional));
    optionalDelete(optional);
}

void testLREFileRead_16(){
    RLEConfig config = {
        .type = RLE1,
        .fileOffset = 0,
        .offsetCount = 1,
        .sequenceSize = 2
    };
    stringBufferAppendBytes(sb, "\x04\x00\x00\x00", 4);
    stringBufferAppendBytes(sb, "\x00\x82\x01", 3);

    file = optionalGet(
        memFileCreate(stringBufferToString(sb), stringBufferSize(sb)));

    uint8_t* data = optionalGet(rleReadFile(file, &config));

    assert_memory_equal("\x01\x01", data, 2);

    memoryRelease(data);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testLRE0Decompress_1, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_2, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_3, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_4, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_5, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_6, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_7, setUp, tearDown),

        cmocka_unit_test_setup_teardown(testLRE1Decompress_1, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_2, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_3, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_4, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_5, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_6, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_7, setUp, tearDown),

        cmocka_unit_test_setup_teardown(testLREFileRead_1, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_2, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_3, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_4, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_5, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_6, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_7, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_8, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_9, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_10, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_11, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_12, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_13, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_14, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_15, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testLREFileRead_16, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/lre.c", tests, NULL, NULL);

    return ret;
}
