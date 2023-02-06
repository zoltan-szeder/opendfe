#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>

#include "system/test_memory.h"
#include "odf/sys/stringbuffer.h"

StringBuffer* sb;
char* str;

int setUp(void** state){
    sb = stringBufferCreate();
    return 0;
}

int tearDown(void** state) {
    memoryRelease(str);
    stringBufferDelete(sb);

    assert_int_equal(0, memoryGetAllocations());
    memoryReleaseAll();

    return 0;
}

void testStringBuffer(){
    str = stringBufferToString(sb);
    assert_string_equal("", str);
}

void testStringBufferWithOneChar(){
    stringBufferAppend(sb, "a");

    assert_int_equal(1, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("a", str);
}

void testStringBufferWithSpecialChars(){
    stringBufferAppendBytes(sb, "\x00\x01\x02\x03\x04", 5);
    stringBufferAppendBytes(sb, "\x05\x06\x07\x08\x09", 5);

    assert_int_equal(10, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", str);
}

void testStringBufferWithDifferentChar(){
    stringBufferAppend(sb, "b");

    assert_int_equal(1, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("b", str);
}

void testStringBufferWithString(){
    stringBufferAppend(sb, "ab");

    assert_int_equal(2, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("ab", str);
}


void testStringBufferMultipleAppend(){
    stringBufferAppend(sb, "a");
    stringBufferAppend(sb, "b");

    assert_int_equal(2, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("ab", str);
}

void testStringBufferIsWillNotOverflow(){
    stringBufferAppend(sb, "abcdefghijklmnopqrstuvwxyz");

    assert_int_equal(26, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("abcdefghijklmnopqrstuvwxyz", str);
}

void testStringBufferIsWillNotOverflowWritingMultipleTimes(){
    stringBufferAppend(sb, "abcdefghij");
    stringBufferAppend(sb, "klmnopqrstuvwxyz");

    assert_int_equal(26, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("abcdefghijklmnopqrstuvwxyz", str);
    assert_int_equal(0, str[26]);
}
void testStringBufferWriteFull(){
    stringBufferAppend(sb, "abcde");
    stringBufferAppend(sb, "fghij");

    assert_int_equal(10, stringBufferSize(sb));
    str = stringBufferToString(sb);
    assert_string_equal("abcdefghij", str);
    assert_int_equal(0, str[10]);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testStringBuffer, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferWithOneChar, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferWithDifferentChar, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferWithSpecialChars, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferWithString, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferMultipleAppend, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferIsWillNotOverflow, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferIsWillNotOverflowWritingMultipleTimes, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testStringBufferWriteFull, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/stringbuffer.c", tests, NULL, NULL);

    return ret;
}
