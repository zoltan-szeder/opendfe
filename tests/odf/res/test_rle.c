#include <stddef.h>
#include <stdlib.h>

#include "test_fixtures.h"

#include "odf/sys/types.h"
#include "odf/res/rle.h"
#include "system/test_memory.h"

int tearDown(void** state){
    assert_int_equal(0, memoryGetAllocations());
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

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testLRE0Decompress_1, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_2, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_3, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_4, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_5, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_6, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE0Decompress_7, NULL, tearDown),

        cmocka_unit_test_setup_teardown(testLRE1Decompress_1, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_2, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_3, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_4, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_5, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_6, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testLRE1Decompress_7, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/lre.c", tests, NULL, NULL);

    return ret;
}
