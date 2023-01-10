#include "test_fixtures.h"
#include "assertions/string.h"
#include "assertions/memory.h"

#include "odf/sys/log.h"

#include <string.h>

#include "odf/sys/file.h"
#include "odf/sys/optional.h"

#define LOG_FILE "tests/resources/test_log.log"
FILE* logStream = NULL;

static FILE* openLog();
static char* readLog();
static void assertLog(char* expectedContent);

void tearDown(){
    fclose(logStream);
    remove(LOG_FILE);
    assertAllMemoryReleased();
}

void setUp() {
    logStream = openLog();
    logSetStream(logStream);
}

void testFatal(){
    testCase("testFatal");

    logSetLevel(FATAL);
    logFatal("fatal: %d", 6);
    logError("error: %d", 5);
    logWarn( "warn:  %d", 4);
    logInfo( "info:  %d", 3);
    logDebug("debug: %d", 2);
    logTrace("trace: %d", 1);

    assertLog("tests/odf/sys/test_log.c:L34 - fatal: 6");
}

void testError(){
    testCase("testError");

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
    testCase("testWarn");

    logSetLevel(WARN);
    logFatal("fatal");
    logError("error");
    logWarn( "warn");
    logInfo( "info");
    logDebug("debug");
    logTrace("trace");

    assertLog(
        "tests/odf/sys/test_log.c:L65 - fatal\n"
        "tests/odf/sys/test_log.c:L66 - error\n"
        "tests/odf/sys/test_log.c:L67 - warn"
    );
}

void testTrace(){
    testCase("testWarn");

    logSetLevel(TRACE);
    logFatal("fatal");
    logError("error");
    logWarn( "warn");
    logInfo( "info");
    logDebug("debug");
    logTrace("trace");

    assertLog(
        "tests/odf/sys/test_log.c:L83 - fatal\n"
        "tests/odf/sys/test_log.c:L84 - error\n"
        "tests/odf/sys/test_log.c:L85 - warn\n"
        "tests/odf/sys/test_log.c:L86 - info\n"
        "tests/odf/sys/test_log.c:L87 - debug\n"
        "tests/odf/sys/test_log.c:L88 - trace"
    );
}

static FILE* openLog() {
    return fopen(LOG_FILE, "w");
}

static void assertLog(char* expectedContent) {
    char* content = readLog();

    assertEquals(expectedContent, content, strlen(expectedContent));
    memoryRelease(content);
}

static char* readLog() {
    return optionalGet(fileReadAll(LOG_FILE));
}

int main(int argc, char** argv){
    void (*testFunctions[])() = {
        &testFatal,
        &testError,
        &testWarn,
        &testTrace,
    };

    TestFixture fixture = createFixture();

    fixture.name = "odf/sys/log.c";
    fixture.beforeEach = &setUp;
    fixture.afterEach = &tearDown;
    fixture.tests = testFunctions;
    fixture.length = sizeof(testFunctions) / sizeof(testFunctions[0]);

    runTests(&fixture);
}
