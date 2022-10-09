#ifndef ASSERTIONS_OPTIONAL_H
#define ASSERTIONS_OPTIONAL_H

#include "test_fixtures.h"
#include "system/test_memory.h"
#include "odf/sys/optional.h"

void assertOptionalIsEmpty(void* opt){
    if(!optionalIsEmpty(opt)) {
        fail(error("Optional is not empty"));
        optionalDelete(opt);
        return;
    }

    char* msg = optionalGetMessage(opt);
    memoryRelease(msg);
}

void assertOptionalNotEmpty(void* opt){
    if(optionalIsEmpty(opt)) {
        char* msg = optionalGetMessage(opt);
        fail(error("Optional is empty:\n%s", msg));
        memoryRelease(msg);
        return;
    }
}

#endif
