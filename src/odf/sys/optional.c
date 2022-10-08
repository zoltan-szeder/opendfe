
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

struct OptionalUInt8 {
    bool isPresent;
    const char* message;
    uint8 value;
};

struct OptionalUInt16 {
    bool isPresent;
    const char* message;
    uint16 value;
};

struct OptionalUInt32 {
    bool isPresent;
    const char* message;
    uint32 value;
};

struct OptionalUInt64 {
    bool isPresent;
    const char* message;
    uint64 value;
};

struct OptionalInt8 {
    bool isPresent;
    const char* message;
    int8 value;
};

struct OptionalInt16 {
    bool isPresent;
    const char* message;
    int16 value;
};

struct OptionalInt32 {
    bool isPresent;
    const char* message;
    int32 value;
};

struct OptionalInt64 {
    bool isPresent;
    const char* message;
    int64 value;
};

Optional MEMORY_ALLOCATION_ISSUE = {
    .isPresent = false,
    .message = "optional.c:optionalCreate - Could not allocate memory for Optional type"
};

void* optionalEmpty(const char* formatString, ...) {
    Optional* optional = memoryAllocate(sizeof(Optional));
    memoryTag(optional, "EmptyOptional");
    optional->isPresent = false;

    optional->message = memoryAllocate(1024*sizeof(char));
    memoryTag(optional->message, "EmptyOptionalMsg");
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
    Optional* optional = memoryAllocate(size);
    memoryTag(optional, "Optional");
    optional->isPresent = true;
    optional->message = NULL;

    return optional;
}

OptionalPtr* optionalOf(void* ptr) {
    OptionalPtr* optional = optionalCreate(sizeof(OptionalPtr));
    optional->value = ptr;
    return optional;
}

OptionalUInt8* optionalUInt8(uint8 value) {
    OptionalUInt8* optional = optionalCreate(sizeof(OptionalUInt8));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalUInt16* optionalUInt16(uint16 value) {
    OptionalUInt16* optional = optionalCreate(sizeof(OptionalUInt16));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalUInt32* optionalUInt32(uint32 value) {
    OptionalUInt32* optional = optionalCreate(sizeof(OptionalUInt32));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalUInt64* optionalUInt64(uint64 value) {
    OptionalUInt64* optional = optionalCreate(sizeof(OptionalUInt64));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}


OptionalInt8* optionalInt8(int8 value) {
    OptionalInt8* optional = optionalCreate(sizeof(OptionalInt8));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalInt16* optionalInt16(int16 value) {
    OptionalInt16* optional = optionalCreate(sizeof(OptionalInt16));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalInt32* optionalInt32(int32 value) {
    OptionalInt32* optional = optionalCreate(sizeof(OptionalInt32));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

OptionalInt64* optionalInt64(int64 value) {
    OptionalInt64* optional = optionalCreate(sizeof(OptionalInt64));
    if(optionalIsEmpty(optional)) {
        return optional;
    }

    optional->value = value;
    return optional;
}

void* optionalGet(OptionalPtr* optional){
    void* value = optional->value;
    optionalDelete(optional);
    return value;
}

uint8 optionalGetUInt8(OptionalUInt8* optional) {
    uint8 value = optional->value;
    optionalDelete(optional);
    return value;
}
uint16 optionalGetUInt16(OptionalUInt16* optional) {
    uint16 value = optional->value;
    optionalDelete(optional);
    return value;
}
uint32 optionalGetUInt32(OptionalUInt32* optional) {
    uint32 value = optional->value;
    optionalDelete(optional);
    return value;
}
uint64 optionalGetUInt64(OptionalUInt64* optional) {
    uint64 value = optional->value;
    optionalDelete(optional);
    return value;
}

int8 optionalGetInt8(OptionalInt8* optional) {
    int8 value = optional->value;
    optionalDelete(optional);
    return value;
}
int16 optionalGetInt16(OptionalInt16* optional) {
    int16 value = optional->value;
    optionalDelete(optional);
    return value;
}
int32 optionalGetInt32(OptionalInt32* optional) {
    int32 value = optional->value;
    optionalDelete(optional);
    return value;
}
int64 optionalGetInt64(OptionalInt64* optional) {
    int64 value = optional->value;
    optionalDelete(optional);
    return value;
}
