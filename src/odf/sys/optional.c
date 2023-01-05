
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "stddef.h"

struct Optional {
    bool isPresent;
    char* message;
};

struct OptionalPtr {
    bool isPresent;
    const char* message;
    void* value;
};

Optional MEMORY_ALLOCATION_ISSUE = {
    .isPresent = false,
    .message = "odf/sys/optional.c:optionalCreate - Could not allocate memory for Optional type"
};

void* optionalEmpty(const char* formatString, ...) {
    Optional* optional = memoryAllocateWithTag(sizeof(Optional), "odf/sys/optional/EmptyOptional");
    optional->isPresent = false;

    optional->message = memoryAllocateWithTag(1024*sizeof(char), "odf/sys/optional/EmptyOptional/message");
    va_list args;
    va_start(args, formatString);
    vsnprintf(optional->message, 1024, formatString, args);
    va_end(args);

    return optional;
}

bool optionalIsEmpty(void* optional) {
    return !((Optional*) optional)->isPresent;
}

Optional* optionalFirstEmpty(int length, ...) {
    if(length == 0)
        return false;

    va_list args;
    va_start(args, length);

    Optional* optional;
    for(int i = 0; i < length; i++) {
        optional = va_arg(args, Optional*);
        if(optionalIsEmpty(optional)) {
            va_end(args);
            return optional;
        }
    }

    va_end(args);

    return NULL;
}

void optionalDelete(void* ptr) {
    Optional* optional = ptr;

    if(optional == &MEMORY_ALLOCATION_ISSUE) return;

    memoryRelease(optional);
}

char* optionalGetMessage(void* ptr) {
    Optional* optional = ptr;
    char* message = optional->message;
    optionalDelete(optional);
    return message;
}

void optionalPrint(FILE* stream, Optional* optional) {
    char* message = optionalGetMessage(optional);
    fprintf(stream, "%s\n", message);
    memoryRelease(message);
}

void* optionalCreate(size_t size) {
    Optional* optional = memoryAllocateWithTag(size, "odf/sys/optional/Optional");
    optional->isPresent = true;
    optional->message = NULL;

    return optional;
}

OptionalPtr* optionalOf(void* ptr) {
    OptionalPtr* optional = optionalCreate(sizeof(OptionalPtr));
    optional->value = ptr;
    return optional;
}


void* optionalGet(OptionalPtr* optional){
    if(!optional->isPresent) {
        fprintf(stderr, "Invoking optionalGet on an empty Optional! Aborting!");
        abort();
    }
    void* value = optional->value;
    optionalDelete(optional);
    return value;
}
