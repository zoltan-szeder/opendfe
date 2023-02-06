#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "test_fixtures.h"
#include "system/test_memory.h"

#include "odf/sys/file.h"
#include "odf/sys/optional.h"


int tearDown(void** state){
    assert_true(memoryGetAllocations() == 0);
    return 0;
}

void testOpenMissingFile(){
    OptionalPtr* optional = fileOpen("missing/nonExistent.file", "rb");
    assert_true(optionalIsEmpty(optional));
    char* msg = optionalGetMessage(optional);
    assert_true(msg != NULL);
    memoryRelease(msg);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testOpenMissingFile, NULL, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/sys/file.c", tests, NULL, NULL);

    return ret;
}
