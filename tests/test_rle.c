#include <stddef.h>
#include <stdlib.h>

#include "test_fixtures.h"

#include "types.h"
#include "drivers/rle.h"

void testLRE0Decompress_1();
void testLRE0Decompress_2();
void testLRE0Decompress_3();
void testLRE0Decompress_4();
void testLRE0Decompress_5();
void testLRE0Decompress_6();
void testLRE0Decompress_7();

void testLRE1Decompress_1();
void testLRE1Decompress_2();
void testLRE1Decompress_3();
void testLRE1Decompress_4();
void testLRE1Decompress_5();
void testLRE1Decompress_6();
void testLRE1Decompress_7();

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testLRE0Decompress_1,
        &testLRE0Decompress_2,
        &testLRE0Decompress_3,
        &testLRE0Decompress_4,
        &testLRE0Decompress_5,
        &testLRE0Decompress_6,
        &testLRE0Decompress_7,

        &testLRE1Decompress_1,
        &testLRE1Decompress_2,
        &testLRE1Decompress_3,
        &testLRE1Decompress_4,
        &testLRE1Decompress_5,
        &testLRE1Decompress_6,
        &testLRE1Decompress_7,
    };

    TestFixture fixture = createFixture();

    fixture.name = "lre.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}

void testLRE0Decompress_1(){
    testCase("LRE0 1x1 image 01");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x01";

    uint8* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assertEquali(image[0], lre[1]);
    free(image);
}

void testLRE0Decompress_2(){
    testCase("LRE0 1x1 image 02");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x02";

    uint8* image = rle0Decompress((unsigned char*) lre, 2, w, h);

    assertEquali(image[0], lre[1]);
    free(image);
}

void testLRE0Decompress_3(){
    testCase("LRE0 2x1 image");

    int w = 2;
    int h = 1;
    char* lre = "\x02\x01\x02";

    uint8* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assertEquali(image[0], lre[1]);
    assertEquali(image[1], lre[2]);

    free(image);
}

void testLRE0Decompress_4(){
    testCase("LRE0 3x1 image 1 2 0");

    int w = 3;
    int h = 1;
    char* lre = "\x02\x01\x02\x81";

    uint8* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assertEquali(image[0], lre[1]);
    assertEquali(image[1], lre[2]);
    assertEquali(image[2], '\x00');

    free(image);
}


void testLRE0Decompress_5(){
    testCase("LRE0 2x2 image 1 2 0 0");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x82";

    uint8* image = rle0Decompress((unsigned char*) lre, 4, w, h);

    assertEquali(image[0], lre[1]);
    assertEquali(image[1], lre[2]);
    assertEquali(image[2], '\x00');
    assertEquali(image[3], '\x00');

    free(image);
}


void testLRE0Decompress_6(){
    testCase("LRE0 2x2 image 1 2 3 4");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x02\x03\x04";

    uint8* image = rle0Decompress((unsigned char*) lre, 6, w, h);

    assertEquali(image[0], '\x01');
    assertEquali(image[1], '\x02');
    assertEquali(image[2], '\x03');
    assertEquali(image[3], '\x04');

    free(image);
}

void testLRE0Decompress_7(){
    testCase("LRE0 2x2 image 0 0 0 4");

    int w = 2;
    int h = 2;
    char* lre = "\x83\x01\x04";

    uint8* image = rle0Decompress((unsigned char*) lre, 3, w, h);

    assertEquali(image[0], '\x00');
    assertEquali(image[1], '\x00');
    assertEquali(image[2], '\x00');
    assertEquali(image[3], '\x04');

    free(image);
}


void testLRE1Decompress_1(){
    testCase("LRE1 1x1 image 01");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x01";

    uint8* image = rle1Decompress((unsigned char*) lre, 2, w, h);

    assertEquali(image[0], lre[1]);
    free(image);
}

void testLRE1Decompress_2(){
    testCase("LRE1 1x1 image 02");

    int w = 1;
    int h = 1;
    char* lre = "\x01\x02";

    uint8* image = rle1Decompress((unsigned char*) lre, 2, w, h);

    assertEquali(image[0], lre[1]);
    free(image);
}

void testLRE1Decompress_3(){
    testCase("LRE1 2x1 image");

    int w = 2;
    int h = 1;
    char* lre = "\x02\x01\x02";

    uint8* image = rle1Decompress((unsigned char*) lre, 3, w, h);

    assertEquali(image[0], lre[1]);
    assertEquali(image[1], lre[2]);

    free(image);
}

void testLRE1Decompress_4(){
    testCase("LRE1 3x1 image 1 2 3");

    int w = 3;
    int h = 1;
    char* lre = "\x02\x01\x02\x81\x03";

    uint8* image = rle1Decompress((unsigned char*) lre, 5, w, h);

    assertEquali(image[0], '\x01');
    assertEquali(image[1], '\x02');
    assertEquali(image[2], '\x03');

    free(image);
}


void testLRE1Decompress_5(){
    testCase("LRE1 2x2 image 1 2 3 3");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x82\x03";

    uint8* image = rle1Decompress((unsigned char*) lre, 5, w, h);

    assertEquali(image[0], '\x01');
    assertEquali(image[1], '\x02');
    assertEquali(image[2], '\x03');
    assertEquali(image[3], '\x03');

    free(image);
}


void testLRE1Decompress_6(){
    testCase("LRE1 2x2 image 1 2 3 4");

    int w = 2;
    int h = 2;
    char* lre = "\x02\x01\x02\x02\x03\x04";

    uint8* image = rle1Decompress((unsigned char*) lre, 6, w, h);

    assertEquali(image[0], '\x01');
    assertEquali(image[1], '\x02');
    assertEquali(image[2], '\x03');
    assertEquali(image[3], '\x04');

    free(image);
}

void testLRE1Decompress_7(){
    testCase("LRE1 2x2 image 0 0 0 4");

    int w = 2;
    int h = 2;
    char* lre = "\x83\x01\x01\x04";

    uint8* image = rle1Decompress((unsigned char*) lre, 4, w, h);

    assertEquali(image[0], '\x01');
    assertEquali(image[1], '\x01');
    assertEquali(image[2], '\x01');
    assertEquali(image[3], '\x04');

    free(image);
}
