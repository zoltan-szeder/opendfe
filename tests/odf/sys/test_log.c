#include "test_fixtures.h"
#include "system/test_memory.h"

#include "odf/sys/log.h"

#include <string.h>

#include "odf/sys/file.h"
#include "odf/sys/optional.h"

#define LOG_FILE "tests/resources/test_log.log"
FILE* logStream = NULL;

static FILE* openLog();
static char* readLog();
static void assertLog(char* expectedContent);


int setUp(void** state) {
    logStream = openLog();
    logSetStream(logStream);

    return 0;
}

int tearDown(void** state){
    fclose(logStream);
    remove(LOG_FILE);
    assert_true(memoryGetAllocations() == 0);

    return 0;
}

void testFatal(){
    logSetLevel(FATAL);
    logFatal("fatal: %d", 6);
    logError("error: %d", 5);
    logWarn( "warn:  %d", 4);
    logInfo( "info:  %d", 3);
    logDebug("debug: %d", 2);
    logTrace("trace: %d", 1);

    assertLog("tests/odf/sys/test_log.c:L36 - fatal: 6");
}

void testError(){
    logSetLevel(ERROR);
    logFatal("fatal");
    logError("error");
    logWarn( "warn");
    logInfo( "info");
    logDebug("debug");
    logTrace("trace");

    assertLog(
        "tests/odf/sys/test_log.c:L48 - fatal\n"
        "tests/odf/sys/test_log.c:L49 - error"
    );
}

void testWarn(){
    logSetLevel(WARN);
    logFatal("fatal");
    logError("error");
    logWarn( "warn");
    logInfo( "info");
    logDebug("debug");
    logTrace("trace");

    assertLog(
        "tests/odf/sys/test_log.c:L63 - fatal\n"
        "tests/odf/sys/test_log.c:L64 - error\n"
        "tests/odf/sys/test_log.c:L65 - warn"
    );
}

void testTrace(){
    logSetLevel(TRACE);
    logFatal("fatal");
    logError("error");
    logWarn( "warn");
    logInfo( "info");
    logDebug("debug");
    logTrace("trace");

    assertLog(
        "tests/odf/sys/test_log.c:L79 - fatal\n"
        "tests/odf/sys/test_log.c:L80 - error\n"
        "tests/odf/sys/test_log.c:L81 - warn\n"
        "tests/odf/sys/test_log.c:L82 - info\n"
        "tests/odf/sys/test_log.c:L83 - debug\n"
        "tests/odf/sys/test_log.c:L84 - trace"
    );
}

static FILE* openLog() {
    return fopen(LOG_FILE, "w");
}

static void assertLog(char* expectedContent) {
    char* content = readLog();

    assert_string_equal(expectedContent, content);
    memoryRelease(content);
}

static char* readLog() {
    return optionalGet(fileReadAll(LOG_FILE));
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testFatal, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testError, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testWarn, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testTrace, setUp, tearDown),\
    };

    int ret = cmocka_run_group_tests_name("odf/sys/log.c", tests, NULL, NULL);

    return ret;
}
