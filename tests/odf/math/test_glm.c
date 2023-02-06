#include "test_fixtures.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>


#include "odf/math/glm.h"

void assertEqualMat(
    mat4 mat,
    float x1, float x2, float x3, float x4,
    float y1, float y2, float y3, float y4,
    float z1, float z2, float z3, float z4,
    float w1, float w2, float w3, float w4
) {
    fvec4 col1 =  mat.cols[0]; fvec4 col2 =  mat.cols[1]; fvec4 col3 =  mat.cols[2]; fvec4 col4 =  mat.cols[3];
    assert_int_equal(x1, col1.x); assert_int_equal(y1, col1.y); assert_int_equal(z1, col1.z); assert_int_equal(w1, col1.w);
    assert_int_equal(x2, col2.x); assert_int_equal(y2, col2.y); assert_int_equal(z2, col2.z); assert_int_equal(w2, col2.w);
    assert_int_equal(x3, col3.x); assert_int_equal(y3, col3.y); assert_int_equal(z3, col3.z); assert_int_equal(w3, col3.w);
    assert_int_equal(x4, col4.x); assert_int_equal(y4, col4.y); assert_int_equal(z4, col4.z); assert_int_equal(w4, col4.w);
}

void testUnitMatrix(){
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
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testUnitMatrix, NULL, NULL),
        cmocka_unit_test_setup_teardown(testTranslate, NULL, NULL),
        cmocka_unit_test_setup_teardown(testScale, NULL, NULL),
        cmocka_unit_test_setup_teardown(testRotateZ180, NULL, NULL),
        cmocka_unit_test_setup_teardown(testPerspective, NULL, NULL),
    };

    int ret = cmocka_run_group_tests_name("odf/math/glm.c", tests, NULL, NULL);

    return ret;
}
