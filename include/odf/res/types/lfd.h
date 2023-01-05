#ifndef ODF_RES_TYPES_LFD_H
#define ODF_RES_TYPES_LFD_H

#include "odf/sys/types.h"

typedef struct LfdArchive LfdArchive;
typedef struct LfdFile LfdFile;

typedef struct {
    char type[4];
    char name[8];
    uint32_t length;
} LfdChunk;

#endif
