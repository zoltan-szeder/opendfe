#ifndef TEST_FIXTURES_H
#define TEST_FIXTURES_H

#include <stdbool.h>
#include <stdarg.h>

typedef struct {
    char* name;
    int length;
    void (*before)();
    void (*beforeEach)();
    void (**tests)();
    void (*after)();
    void (*afterEach)();
} TestFixture;

typedef struct {
    char* msg;
} TestError;

TestFixture createFixture();
bool runTests(TestFixture*);

void testCase(const char* testName);

void assertEqual(long, long);
void assertTrue(bool);
void assertTrueMsg(bool, char*);

char* error(const char*, ...);

#endif
