#ifndef ODF_SYS_OPTIONAL_H
#define ODF_SYS_OPTIONAL_H

#include "odf/sys/types/optional.h"

#include <stdio.h>
#include "stdbool.h"
#include "odf/sys/types.h"

void* optionalEmpty(const char* message, ...);
bool optionalIsEmpty(void*);
Optional* optionalFirstEmpty(int length, ...);
void optionalDelete(void*);
char* optionalGetMessage(void*);
void optionalPrint(FILE* stream, Optional* optional);

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
