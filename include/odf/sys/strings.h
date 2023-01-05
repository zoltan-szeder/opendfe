#ifndef ODF_SYS_STRINGS_H
#define ODF_SYS_STRINGS_H

#include <stdio.h>
#include "odf/sys/list.h"

long findFirstFrom(const char* str, char ch, size_t offset);
size_t getBlockLenghtFromFormat(const char* format);

List* strSplitByLineBreak(char* content);

#endif
