#ifndef TEST_FIXTURES_H
#define TEST_FIXTURES_H

typedef struct {
    char* name;
    int length;
    void (*before)();
    void (*beforeEach)();
    void (**tests)();
    void (*after)();
    void (*afterEach)();
} TestFixture;

TestFixture createFixture();
void runTests(TestFixture*);

#endif
