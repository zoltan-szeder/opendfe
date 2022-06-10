#ifndef IMG_H
#define IMG_H

#include "math/vec.h"
#include "types.h"

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
