#include <stddef.h>
#include <stdlib.h>

#include "test_fixtures.h"

#include "types.h"
#include "drivers/rle.h"

void testLREDecompress_1();
void testLREDecompress_2();
void testLREDecompress_3();
void testLREDecompress_4();
void testLREDecompress_5();
void testLREDecompress_6();
void testLREDecompress_7();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testLREDecompress_1,
        &testLREDecompress_2,
        &testLREDecompress_3,
        &testLREDecompress_4,
        &testLREDecompress_5,
        &testLREDecompress_6,
        &testLREDecompress_7,
    };

    TestFixture fixture = createFixture();

    fixture.name = "lre.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testLREDecompress_1(){
    testCase("1x1 image 01");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x01";

    uint8* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assertEqual(image[0], lre[1]);
    free(image);
}

void testLREDecompress_2(){
    testCase("1x1 image 02");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x02";

    uint8* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assertEqual(image[0], lre[1]);
    free(image);
}

void testLREDecompress_3(){
    testCase("2x1 image");

    int w = 2;
    int h = 1;
    char* lre = "\x02\x01\x02";

    uint8* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assertEqual(image[0], lre[1]);
    assertEqual(image[1], lre[2]);

    free(image);
}

void testLREDecompress_4(){
    testCase("3x1 image 1 2 0");

    int w = 3;
    int h = 1;
    char* lre = "\x02\x01\x02\x81";

    uint8* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assertEqual(image[0], lre[1]);
    assertEqual(image[1], lre[2]);
    assertEqual(image[2], '\x00');

    free(image);
}


void testLREDecompress_5(){
    testCase("2x2 image 1 2 0 0");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x82";

    uint8* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assertEqual(image[0], lre[1]);
    assertEqual(image[1], lre[2]);
    assertEqual(image[2], '\x00');
    assertEqual(image[3], '\x00');

    free(image);
}


void testLREDecompress_6(){
    testCase("2x2 image 1 2 3 4");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x02\x03\x04";

    uint8* image = rle0Decompress((unsigned char*) lre, 6, w, h);

    assertEqual(image[0], '\x01');
    assertEqual(image[1], '\x02');
    assertEqual(image[2], '\x03');
    assertEqual(image[3], '\x04');

    free(image);
}

void testLREDecompress_7(){
    testCase("2x2 image 0 0 0 4");

    int w = 2;
    int h = 2;
    char* lre = "\x83\x01\x04";

    uint8* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assertEqual(image[0], '\x00');
    assertEqual(image[1], '\x00');
    assertEqual(image[2], '\x00');
    assertEqual(image[3], '\x04');

    free(image);
}
