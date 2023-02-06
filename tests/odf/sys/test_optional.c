#define CMOCKA_TEST
#include "test_fixtures.h"
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "system/test_memory.h"

#include "odf/sys/optional.h"

#include <string.h>

typedef struct {
    uint8_t c;
    uint32_t n;
} TestStruct;

int tearDown(void** state){
    assert_int_equal(0, memoryGetAllocations());

    if(!memoryIsEmpty()) {
        memoryDump(false);
    }

    return 0;
}

void testOptional(){
    TestStruct ts = {
        .c = 'c',
        .n = 32,
    };

    OptionalPtr* optional = optionalOf(&ts);

    assert_false(optionalIsEmpty(optional));

    TestStruct* tsPtr = optionalGet(optional);

    assert_int_equal(tsPtr->c, ts.c);
    assert_int_equal(tsPtr->n, ts.n);
}

void testEmptyOptional(){
    OptionalPtr* optional = optionalEmpty("testEmptyOptional - This optional is empty");

    assert_true(optionalIsEmpty(optional));

    char* msg = optionalGetMessage(optional);
    char* expectedMessage = "tests/odf/sys/test_optional.c:L45:"
        "testEmptyOptional - This optional is empty";
    assert_string_equal(expectedMessage, msg);

    memoryRelease(msg);
}

void testEmptyOptionalWithParameters(){
    OptionalPtr* optional = optionalEmpty("testEmptyOptionalWithParameters - This optional is \"%s\"", "empty");

    assert_true(optionalIsEmpty(optional));

    char* msg = optionalGetMessage(optional);
    char* expectedMessage = "tests/odf/sys/test_optional.c:L58:"
    "testEmptyOptionalWithParameters - This optional is \"empty\"";
    assert_string_equal(expectedMessage, msg);

    memoryRelease(msg);
}

void testOptionalCanBeDeleted(){
    TestStruct ts = {
        .c = 'c',
        .n = 32,
    };

    OptionalPtr* optional = optionalOf(&ts);

    optionalDelete(optional);
}

void testEmptyOptionalCanBeDeleted(){
    OptionalPtr* optional = optionalEmpty("message");

    optionalDelete(optional);
}

void testOptionalOfNullIsAnEmptyOptional(){
    OptionalPtr* optional = optionalOf(NULL);

    assert_true(optionalIsEmpty(optional));

    char* msg = optionalGetMessage(optional);

    assert_true(strlen(msg) > 0);

    memoryRelease(msg);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testOptional, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testEmptyOptional, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testEmptyOptionalWithParameters, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testOptionalCanBeDeleted, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testEmptyOptionalCanBeDeleted, NULL, tearDown),
        cmocka_unit_test_setup_teardown(testOptionalOfNullIsAnEmptyOptional, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/optional.c", tests, NULL, NULL);

    return ret;
}
