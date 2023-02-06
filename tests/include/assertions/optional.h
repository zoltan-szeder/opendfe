#ifndef ASSERTIONS_OPTIONAL_H
#define ASSERTIONS_OPTIONAL_H

#include "test_fixtures.h"
#include "system/test_memory.h"
#include "odf/sys/optional.h"

void assertOptionalIsEmpty(void* opt){
    if(!optionalIsEmpty(opt)) {
        fail();
        optionalDelete(opt);
        return;
    }

    char* msg = optionalGetMessage(opt);
    memoryRelease(msg);
}

void assertOptionalNotEmpty(void* opt){
    if(optionalIsEmpty(opt)) {
        char* msg = optionalGetMessage(opt);
        fail();
        memoryRelease(msg);
        return;
    }
}

#endif
