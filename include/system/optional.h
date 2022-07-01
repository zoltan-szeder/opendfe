#ifndef SYSTEM_OPTIONAL_H
#define SYSTEM_OPTIONAL_H

#include "stdbool.h"
#include "types.h"

typedef struct {
    bool isPresent;
    char* message;
} Optional;

typedef struct {
    bool isPresent;
    const char* message;
    void* value;
} OptionalPtr;

typedef struct {
    bool isPresent;
    const char* message;
    uint8 value;
} OptionalUInt8;

typedef struct {
    bool isPresent;
    const char* message;
    uint16 value;
} OptionalUInt16;

typedef struct {
    bool isPresent;
    const char* message;
    uint32 value;
} OptionalUInt32;

typedef struct {
    bool isPresent;
    const char* message;
    uint64 value;
} OptionalUInt64;

typedef struct {
    bool isPresent;
    const char* message;
    int8 value;
} OptionalInt8;

typedef struct {
    bool isPresent;
    const char* message;
    int16 value;
} OptionalInt16;

typedef struct {
    bool isPresent;
    const char* message;
    int32 value;
} OptionalInt32;

typedef struct {
    bool isPresent;
    const char* message;
    int64 value;
} OptionalInt64;


void* optionalEmpty(const char* message, ...);
bool optionalIsEmpty(void*);
void optionalDelete(void*);
char* optionalGetMessage(void*);

OptionalPtr* optionalOf(void* ptr);
OptionalUInt8* optionalUInt8(uint8);
OptionalUInt16* optionalUInt16(uint16);
OptionalUInt32* optionalUInt32(uint32);
OptionalUInt64* optionalUInt64(uint64);
OptionalInt8* optionalInt8(int8);
OptionalInt16* optionalInt16(int16);
OptionalInt32* optionalInt32(int32);
OptionalInt64* optionalInt64(int64);

void* optionalGet(OptionalPtr*);
uint8 optionalGetUInt8(OptionalUInt8*);
uint16 optionalGetUInt16(OptionalUInt16*);
uint32 optionalGetUInt32(OptionalUInt32*);
uint64 optionalGetUInt64(OptionalUInt64*);
int8 optionalGetInt8(OptionalInt8*);
int16 optionalGetInt16(OptionalInt16*);
int32 optionalGetInt32(OptionalInt32*);
int64 optionalGetInt64(OptionalInt64*);


#endif
