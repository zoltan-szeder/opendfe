#ifndef ODF_RES_RLE_H
#define ODF_RES_RLE_H

#include "odf/sys/types.h"

uint8* rle0Decompress(uint8* data, int length, int width, int height);

uint8* rle1Decompress(uint8* data, int length, int width, int height);

#endif
