#ifndef ODF_RES_TYPES_FME_H
#define ODF_RES_TYPES_FME_H

#include "odf/sys/types.h"

#define FME_FLIP_NONE 0
#define FME_FLIP_HORIZONTALLY 1
#define FME_FLIP_VERTICALLY 2

#define FME_COMPRESS_NONE 0
#define FME_COMPRESS_RLE0 1

typedef struct FMEHeader FMEHeader;
typedef struct FMESubHeader FMESubHeader;
typedef struct FMEFile FMEFile;

#endif
