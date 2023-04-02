#ifndef ODF_RES_RLE_H
#define ODF_RES_RLE_H

#include "odf/res/types/lre.h"

#include <stdint.h>

#include "odf/sys/types.h"
#include "odf/sys/types/inmemoryfile.h"
#include "odf/sys/types/optional.h"

OptionalOf(uint8_t*)* rleReadFile(InMemoryFile* file, RLEConfig* config);

uint8_t* rle0Decompress(uint8_t* data, int32_t length, int32_t width, int32_t height);

uint8_t* rle1Decompress(uint8_t* data, int32_t length, int32_t width, int32_t height);

#endif
