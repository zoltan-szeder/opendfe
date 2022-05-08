#include "../headers/asserts.h"
#include <stdio.h>
#include <stdbool.h>
#include <execinfo.h>
#include <stdlib.h>

void print_stacktrace() {
    void* callstack[128];
    int i, frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    for (i = 0; i < frames; ++i) {
        printf("\t%s\n", strs[i]);
    }
    free(strs);
}

void assert_true(bool flag, char* msg) {
    if(!flag) {
        printf("Assertion Failed: %s\n", msg);
        print_stacktrace();
    }
}

void assert_equals(uint32 expected, uint32 actual) {
    char msg[1024];
    
    sprintf(msg, "%d != %d", expected, actual);
    assert_true(expected == actual, msg);
}
