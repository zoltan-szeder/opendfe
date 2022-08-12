#ifndef FILE_H
#define FILE_H
#include <stdio.h>

#include "system/optional.h"


OptionalPtr* fileOpen(char* filePath, char* modifiers);
OptionalPtr* fileReadAll(char* filePath);

OptionalPtr* fileReadBytes(FILE* stream, size_t length);
OptionalPtr* fileReadStruct(FILE* stream, const char* format);
OptionalUInt8* fileReadLittleEndianUInt8(FILE* stream);
OptionalUInt16* fileReadLittleEndianUInt16(FILE* stream);
OptionalUInt32* fileReadLittleEndianUInt32(FILE* stream);
OptionalUInt64* fileReadLittleEndianUInt64(FILE* stream);
OptionalInt8* fileReadLittleEndianInt8(FILE* stream);
OptionalInt16* fileReadLittleEndianInt16(FILE* stream);
OptionalInt32* fileReadLittleEndianInt32(FILE* stream);
OptionalInt64* fileReadLittleEndianInt64(FILE* stream);

OptionalUInt8* fileReadBigEndianUInt8(FILE* stream);
OptionalUInt16* fileReadBigEndianUInt16(FILE* stream);
OptionalUInt32* fileReadBigEndianUInt32(FILE* stream);
OptionalUInt64* fileReadBigEndianUInt64(FILE* stream);
OptionalInt8* fileReadBigEndianInt8(FILE* stream);
OptionalInt16* fileReadBigEndianInt16(FILE* stream);
OptionalInt32* fileReadBigEndianInt32(FILE* stream);
OptionalInt64* fileReadBigEndianInt64(FILE* stream);

#endif
