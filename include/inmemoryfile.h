#ifndef IN_MEMORY_FILE_H
#define IN_MEMORY_FILE_H

#include "types.h"

typedef struct {
    uint32 length;
    char* content;
} InMemoryFile;

#endif
