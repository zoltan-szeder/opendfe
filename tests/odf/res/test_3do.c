#include "odf/res/3do.h"

#include "test_fixtures.h"

#include "odf/sys/log.h"

int setUp(void** state){
    logSetLevel(WARN);

    return 0;
}

int tearDown(void** state){
    return 0;
}

void test3doReadFile() {


}


int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test3doReadFile, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/res/3do.c", tests, NULL, NULL);

    return ret;
}
