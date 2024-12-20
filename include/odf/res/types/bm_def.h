#ifndef ODF_RES_TYPES_BM_DEF_H
#define ODF_RES_TYPES_BM_DEF_H

#include <stdint.h>
#include "odf/res/types/bm.h"
#include "odf/sys/types.h"
#include "odf/sys/types/list.h"

#define BM_NORMAL 0x36
#define BM_TRANSPARENT 0x3e
#define BM_WEAPON 0x08

#define BM_COMPRESSION_NONE 0
#define BM_COMPRESSION_RLE0 2
#define BM_COMPRESSION_RLE1 1


#define BM_HEADER_FORMAT "%c4 %l2 %l2 %l2 %l2 %c1 %c1 %l2 %l4 %c12"

struct BMHeader {
    char magic[4];        // "BM \x1e"
    uint16_t sizeX;       // 1: multiple BM in the file if y > 1; otherwise image width
    uint16_t sizeY;       // image height
    uint16_t idemX;       // N/A
    uint16_t idemY;       // N/A
    uint8_t transparent;  // '\x36': normal; '\x3e': transparent; '\x08' weapon
    uint8_t logSizeY;     // 0 if it's weapon; otherwise log2(sizeY)
    uint16_t compressed;  // 0: not compressed; 1: RLE1 compressed; 2: LRE0 compressed
    uint32_t dataSize;    // byte length of data after header
    uint8_t pad[12];      // 12 byte of '\x00'
};

#define BM_SUBHEADER_FORMAT "%l2 %l2 %l2 %l2 %l4 %c1 %c3 %c8 %l1 %c3"

struct BMSubHeader {
    uint16_t sizeX;       // 1: multiple BM in the file if y > 1; otherwise image width
    uint16_t sizeY;       // image height
    uint16_t idemX;       // N/A
    uint16_t idemY;       // N/A
    uint32_t dataSize;    // byte length of data after header
    uint8_t logSizeY;     // 0 if it's weapon; otherwise log2(sizeY)
    uint8_t pad1[11];     // unused
    uint8_t transparent;  // '0x36': normal; '0x3e': transparent
    uint8_t pad2[3];      // unused
};

struct BMSubFile {
    BMSubHeader* header;
    uint8_t* data;
};

struct BMFile {
    BMHeader* header;
    uint16_t frameRate;
    ListOf(SubBMFile*)* subBMFiles;
    uint8_t* data;
};
#endif
