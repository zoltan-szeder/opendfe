#ifndef ODF_RES_TYPES_FNT_DEF_H
#define ODF_RES_TYPES_FNT_DEF_H

#include "odf/res/types/fnt.h"

#include <stdint.h>

#include "odf/sys/types/list.h"

struct FNTFile {
    FNTHeader* header;
    ListOf(FntChar*)* chars;
};

#define FNT_HEADER_FORMAT "%c4 %c1 %c1 %l2 %c1 %c1 %c22"
struct FNTHeader {
    char magic[4];
    uint8_t height;
    uint8_t _u1;
    uint16_t dataSize;
    char firstChar;
    char lastChar;
    char pad[22];
};

struct FNTChar {
    uint8_t width;
    uint8_t* data;
};

#endif
