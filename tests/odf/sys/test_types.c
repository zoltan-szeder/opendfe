
#include "odf/sys/types.h"
#include "test_fixtures.h"

void testReverseEndianness_1() {
    uint8_t i = 1;
    reverseEndianness(&i, sizeof(uint8_t));
    assert_int_equal(1, i);  // \x01
}


void testReverseEndianness_2() {
    uint16_t i = 1;
    uint8_t* pi = (uint8_t*) &i;
    reverseEndianness(pi, sizeof(uint16_t));
    assert_int_equal(256, i);  // \x01 \x00
}

void testReverseEndianness_3() {
    uint8_t i[] = {'a', 'b', 'c'};
    uint8_t exp[] = {'c', 'b', 'a'};
    reverseEndianness(i, 3*sizeof(uint8_t));
    assert_memory_equal((char*) exp, (char*) i, 3);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testReverseEndianness_1, NULL, NULL),
        cmocka_unit_test_setup_teardown(testReverseEndianness_2, NULL, NULL),
        cmocka_unit_test_setup_teardown(testReverseEndianness_3, NULL, NULL),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/types.c", tests, NULL, NULL);

    return ret;
}
