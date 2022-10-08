#ifndef ODF_SYS_TYPES_H
#define ODF_SYS_TYPES_H

#include "stdbool.h"

#define uint8 unsigned char
#define int8 char
#define uint16 unsigned short
#define int16 short
#define uint32 unsigned int
#define int32 int
#define uint64 unsigned long
#define int64 long

void setToLittleEndian();
void setToBigEndian();
void setToOriginalEndianness();

bool isLittleEndian();
bool isBigEndian();

void reverseEndianness(void* bytes, int lenght);
void* modifyEndiannessOfStruct(void* object, const char* format);

#endif