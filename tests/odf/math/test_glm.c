#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "test_fixtures.h"

#include "odf/math/glm.h"

void assertEqualMat(
    mat4 mat,
    float x1, float x2, float x3, float x4,
    float y1, float y2, float y3, float y4,
    float z1, float z2, float z3, float z4,
    float w1, float w2, float w3, float w4
) {
    fvec4 col1 =  mat.cols[0]; fvec4 col2 =  mat.cols[1]; fvec4 col3 =  mat.cols[2]; fvec4 col4 =  mat.cols[3];
    assertEquali(x1, col1.x); assertEquali(y1, col1.y); assertEquali(z1, col1.z); assertEquali(w1, col1.w);
    assertEquali(x2, col2.x); assertEquali(y2, col2.y); assertEquali(z2, col2.z); assertEquali(w2, col2.w);
    assertEquali(x3, col3.x); assertEquali(y3, col3.y); assertEquali(z3, col3.z); assertEquali(w3, col3.w);
    assertEquali(x4, col4.x); assertEquali(y4, col4.y); assertEquali(z4, col4.z); assertEquali(w4, col4.w);
}

void testUnitMatrix(){
    testCase("testUnitMatrix");

    mat4 unit = mat4Unit();

    assertEqualMat(
        unit,
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

void testTranslate(){
    testCase("testTranslate");

    mat4 unit = mat4Unit();
    fvec3 vec = {.x = 10, .y = 5, .z = 30};
    mat4 translated = mat4Translate(unit, vec);

    assertEqualMat(
        translated,
         1,  0,  0, 10,
         0,  1,  0,  5,
         0,  0,  1, 30,
         0,  0,  0,  1
    );
}

void testScale(){
    testCase("testScale");

    mat4 unit = mat4Unit();
    fvec3 vec = {.x = 10, .y = 5, .z = 30};
    mat4 translated = mat4Scale(unit, vec);

    assertEqualMat(
        translated,
        10,  0,  0,  0,
         0,  5,  0,  0,
         0,  0, 30,  0,
         0,  0,  0,  1
    );
}

void testRotateZ180(){
    testCase("testRotateZ180");

    mat4 unit = mat4Unit();
    fvec3 vec = {.x = 0, .y = 0, .z = 1};
    mat4 translated = mat4Rotate(unit, 180.0f, vec);

    assertEqualMat(
        translated,
        -1,  0,  0,  0,
         0, -1,  0,  0,
         0,  0,  1,  0,
         0,  0,  0,  1
    );
}

void testPerspective(){
    testCase("testPerspective");

    mat4 perspective = mat4Perspective(45.f, 800.f/600.f, 0.1f, 100.f);

    assertEqualMat(
        perspective,
         1,  0,  0,  0,
         0,  2,  0,  0,
         0,  0, -1,  0,
         0,  0, -1,  0
    );
}


int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testUnitMatrix,
        &testTranslate,
        &testScale,
        &testRotateZ180,
        &testPerspective,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/math/glm.c";
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    return runTests(&fixture);
}
