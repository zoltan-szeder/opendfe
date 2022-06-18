#ifndef TYPES_H
#define TYPES_H

#include "stdbool.h"

#define uint8 unsigned char
#define int8 char
#define uint16 unsigned short
#define int16 short
#define uint32 unsigned int
#define int32 int
#define uint64 unsigned long
#define int64 long

bool isLittleEndian();
bool isBigEndian();
void reverseEndianness(uint8* bytes, int lenght);

#endif
