
#include "odf/sys/types.h"
#include "test_fixtures.h"

void testReverseEndianness_1() {
    testCase("Reverse Endianness 1 byte");
    uint8_t i = 1;
    reverseEndianness(&i, sizeof(uint8_t));
    assertEquali(1, i);  // \x01
}


void testReverseEndianness_2() {
    testCase("Reverse Endianness 2 byte");
    uint16_t i = 1;
    uint8_t* pi = (uint8_t*) &i;
    reverseEndianness(pi, sizeof(uint16_t));
    assertEquali(256, i);  // \x01 \x00
}

void testReverseEndianness_3() {
    testCase("Reverse Endianness 3 byte");
    uint8_t i[] = {'a', 'b', 'c'};
    uint8_t exp[] = {'c', 'b', 'a'};
    reverseEndianness(i, 3*sizeof(uint8_t));
    assertEquals((char*) exp, (char*) i, 3);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testReverseEndianness_1,
        &testReverseEndianness_2,
        &testReverseEndianness_3,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/types.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
