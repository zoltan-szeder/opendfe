#ifndef ODF_SYS_IMG_H
#define ODF_SYS_IMG_H

#include "odf/math/vec.h"
#include "odf/sys/types.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t channels;
    uint32_t size;
    uint8_t* data;
} Image8Bit;


Image8Bit* img8bCreate2D(uint32_t width, uint32_t height, uint32_t channels);

void img8bDelete(Image8Bit*);

#endif
