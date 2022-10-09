#ifndef ODF_SYS_STRING_BUFFER_H
#define ODF_SYS_STRING_BUFFER_H

#include <stdlib.h>
#include "odf/sys/types/stringbuffer.h"

StringBuffer* stringBufferCreate();

void stringBufferAppend(StringBuffer* sb, const char* format, ...);
void stringBufferAppendBytes(StringBuffer* sb, const void* bytes, size_t size);

char* stringBufferToString(StringBuffer* sb);
size_t stringBufferSize(StringBuffer* sb);

void stringBufferDelete(StringBuffer* sb);

#endif
