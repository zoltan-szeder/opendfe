#ifndef ODF_SYS_FILE_H
#define ODF_SYS_FILE_H

#include <stdio.h>
#include "odf/sys/types/optional.h"


OptionalPtr* fileOpen(char* filePath, char* modifiers);
OptionalPtr* fileReadAll(char* filePath);

OptionalPtr* fileReadBytes(FILE* stream, size_t length);
OptionalPtr* fileReadStruct(FILE* stream, const char* format);

#endif
