#ifndef ODF_RES_TYPES_BM_H
#define ODF_RES_TYPES_BM_H

#include "types.h"

#define BM_NORMAL 0x36
#define BM_TRANSPARENT 0x3e
#define BM_WEAPON 0x08

#define BM_COMPRESSION_NONE 0
#define BM_COMPRESSION_RLE0 2
#define BM_COMPRESSION_RLE1 1

typedef struct {
    char magic[4];      // "BM \x01e"
    uint16 sizeX;       // 1: multiple BM in the file; image width
    uint16 sizeY;       // image height
    uint16 idemX;       // N/A
    uint16 idemY;       // N/A
    uint8 transparent;  // '\x36': normal; '\x3e': transparent; '\x08' weapon
    uint8 logSizeY;     // 0: weapon; log2(sizeY)
    uint16 compressed;  // 0: not compressed; 1: RLE compressed;
    uint32 dataSize;    // byte length of data after header
    uint8 pad[12];      // 12 x '\x00'
} BMHeader;

typedef struct {
    BMHeader* header;
    uint8* data;        // sizeY * sizeX color reference of a .PAL palette
                        // transparent == '\x3e': '\x00' is transparent
                        // transparent == '\x08': '\x08' is transparent
} BMFile;
#endif
