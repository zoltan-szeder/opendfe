#ifndef ODF_SYS_TYPES_H
#define ODF_SYS_TYPES_H

#include "stdbool.h"
#include "stdint.h"

void setToLittleEndian();
void setToBigEndian();
void setToOriginalEndianness();

bool isLittleEndian();
bool isBigEndian();

void reverseEndianness(void* bytes, int lenght);
void* modifyEndiannessOfStruct(void* object, const char* format);

#endif
