#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_fixtures.h"

typedef struct {
    const char* name;
    bool passed;
    char* errors[32];
    int size;
} TestCaseResult;

typedef struct {
    TestCaseResult* results;
    bool passed;
    int length;
} TestScenario;

TestScenario TEST_SCENARIO;
TestCaseResult* CURRENT_TEST_RESULT;

void runTest(TestFixture*, int);

void printScenarioResults(TestScenario* scenario);
void printTestResult(TestCaseResult* result);
void printError(char* error);

TestFixture createFixture(){
    TestFixture fixture;
    fixture.name = "";
    fixture.beforeEach = NULL;
    fixture.before = NULL;
    fixture.tests = NULL;
    fixture.length = 0;
    fixture.after = NULL;
    fixture.afterEach = NULL;

    return fixture;
}

bool runTests(TestFixture* fixture){
    TestScenario* scenario = &TEST_SCENARIO;
    scenario->results = calloc(fixture->length, sizeof(TestCaseResult));
    scenario->passed = true;
    scenario->length = fixture->length;

    printf("%s ", fixture->name);

    if (fixture->before != NULL) fixture->before();

    for(int i = 0; i < fixture->length; i++) {
        runTest(fixture, i);
    }

    if (fixture->after != NULL) fixture->after();

    printScenarioResults(scenario);

    for(int i = 0; i < TEST_SCENARIO.length; i++) {
        TestCaseResult* result = TEST_SCENARIO.results + i;
        for(int j = 0; j < result->size; j++) {
            free(result->errors[j]);
        }
    }
    free(TEST_SCENARIO.results);

    return !TEST_SCENARIO.passed;
}

void runTest(TestFixture* fixture, int testNumber) {
    CURRENT_TEST_RESULT = &(TEST_SCENARIO.results[testNumber]);
    void (*test)() = fixture->tests[testNumber];

    if (fixture->beforeEach != NULL) fixture->beforeEach();
    CURRENT_TEST_RESULT->passed = true;
    (*test)();
    if (fixture->afterEach != NULL) fixture->afterEach();

    if(CURRENT_TEST_RESULT->passed) {
        printf(".");
    } else {
        printf("F");
    }
}

void testCase(const char* testName) {
    CURRENT_TEST_RESULT->name = testName;
}

void assertEquali(long c1, long c2) {
    assertTrueMsg(c1 == c2, error("%ld != %ld", c1, c2));
}

void assertEquals(char* c1, char* c2, int length) {
    char* ntc1 = malloc((length+1)*sizeof(char));
    char* ntc2 = malloc((length+1)*sizeof(char));
    memcpy(ntc1, c1, length);
    ntc1[length] = 0;
    memcpy(ntc2, c2, length);
    ntc2[length] = 0;
    assertTrueMsg(strncmp(ntc1, ntc2, length+1) == 0, error("\"%s\" != \"%s\"", ntc1, ntc2));
    free(ntc1);
    free(ntc2);
}

void assertTrue(bool flag) {
    assertTrueMsg(flag, error("false != true"));
}

void assertTrueMsg(bool flag, char* error) {
    if(flag || CURRENT_TEST_RESULT->size >= 32) {
        free(error);
        return;
    };

    CURRENT_TEST_RESULT->passed = false;
    TEST_SCENARIO.passed = false;
    CURRENT_TEST_RESULT->errors[CURRENT_TEST_RESULT->size] = error;
    CURRENT_TEST_RESULT->size++;
}

void assertFalse(bool flag) {
    assertTrue(!flag);
}

void assertFalseMsg(bool flag, char* msg) {
    assertTrueMsg(!flag, msg);
}

void fail(char* msg) {
    assertTrueMsg(false, msg);
}

char* error(const char* formatString, ...) {
    char* msg = malloc(1024*sizeof(char));

    va_list args;
    va_start(args, formatString);

    vsnprintf(msg, 1024, formatString, args);

    va_end(args);

    return msg;
}

void printScenarioResults(TestScenario* scenario) {
    if(scenario->passed) {
        printf(" [PASS]\n");
    } else {
        printf(" [FAIL]\n");
    }

    for(int i = 0; i < scenario->length; i++) {
        TestCaseResult* result = scenario->results + i;
        if(!result->passed){
            printTestResult(scenario->results + i);
        }
    }
}

void printTestResult(TestCaseResult* result) {
    if(result->name != NULL) {
        fprintf(stderr, "Case: \"%s\"\n", result->name);
    } else {
        fprintf(stderr, "Case: Unknown, please provide name for test case\n");
    }
    for(int i = 0; i < result->size; i++) {
        printError(result->errors[i]);
    }
}

void printError(char* error) {
    fprintf(stderr, "%s\n", error);
}
