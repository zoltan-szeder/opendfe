#ifndef FILE_H
#define FILE_H
#include <stdio.h>

#include "system/optional.h"


OptionalPtr* fileOpen(char* filePath, char* modifiers);
OptionalPtr* fileReadAll(char* filePath);

OptionalPtr* fileReadBytes(FILE* stream, size_t length);
OptionalPtr* fileReadStruct(FILE* stream, const char* format);

#endif
