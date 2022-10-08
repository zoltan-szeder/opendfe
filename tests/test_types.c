
#include "odf/sys/types.h"
#include "test_fixtures.h"

void testReverseEndianness_1() {
    testCase("Reverse Endianness 1 byte");
    uint8 i = 1;
    reverseEndianness(&i, sizeof(uint8));
    assertEquali(1, i);  // \x01
}


void testReverseEndianness_2() {
    testCase("Reverse Endianness 2 byte");
    uint16 i = 1;
    uint8* pi = (uint8*) &i;
    reverseEndianness(pi, sizeof(uint16));
    assertEquali(256, i);  // \x01 \x00
}

void testReverseEndianness_3() {
    testCase("Reverse Endianness 3 byte");
    uint8 i[] = {'a', 'b', 'c'};
    uint8 exp[] = {'c', 'b', 'a'};
    reverseEndianness(i, 3*sizeof(uint8));
    assertEquals((char*) exp, (char*) i, 3);
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testReverseEndianness_1,
        &testReverseEndianness_2,
        &testReverseEndianness_3,
    };

    TestFixture fixture = createFixture();

    fixture.name = "types.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
