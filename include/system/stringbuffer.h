#ifndef SYSTEM_STRING_BUFFER_H
#define SYSTEM_STRING_BUFFER_H

#include <stdlib.h>

typedef struct StringBuffer StringBuffer;

StringBuffer* stringBufferCreate(size_t size);

void stringBufferAppend(StringBuffer* sb, const char* format, ...);
char* stringBufferToString(StringBuffer* sb);
size_t stringBufferSize(StringBuffer* sb);

void stringBufferDelete(StringBuffer* sb);



#endif
