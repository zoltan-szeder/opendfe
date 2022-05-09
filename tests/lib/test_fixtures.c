#include <stddef.h>
#include "../headers/test_fixtures.h"

void runTest(TestFixture*, int);

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

void runTests(TestFixture* fixture){
    printf("%s ", fixture->name);

    if (fixture->before != NULL) fixture->before();

    for(int i = 0; i < fixture->length; i++) {
        runTest(fixture, i);
    }

    if (fixture->after != NULL) fixture->after();

    printf(" [PASS]\n");
}

void runTest(TestFixture* fixture, int testNumber) {
    void (*test)() = fixture->tests[testNumber];

    if (fixture->beforeEach != NULL) fixture->beforeEach();
    (*test)();
    if (fixture->afterEach != NULL) fixture->afterEach();

    printf(".");
}
