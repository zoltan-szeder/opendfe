#ifndef ODF_SYS_IMG_H
#define ODF_SYS_IMG_H

#include "math/vec.h"
#include "odf/sys/types.h"

typedef struct {
    uint32 width;
    uint32 height;
    uint32 depth;
    uint32 channels;
    uint32 size;
    uint8* data;
} Image8Bit;


Image8Bit* img8bCreate2D(uint32 width, uint32 height, uint32 channels);

void img8bDelete(Image8Bit*);

#endif
