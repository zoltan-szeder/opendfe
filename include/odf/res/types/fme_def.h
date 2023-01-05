#ifndef ODF_RES_TYPES_FME_DEF_H
#define ODF_RES_TYPES_FME_DEF_H

#include <stdint.h>

#define FME_FLIP_NONE 0
#define FME_FLIP_HORIZONTALLY 1
#define FME_FLIP_VERTICALLY 2

#define FME_COMPRESS_NONE 0
#define FME_COMPRESS_RLE0 1


#define FME_HEADER_FORMAT "%l4 %l4 %l4 %l4 %l4 %l4 %c8"
struct FMEHeader {
    uint32_t insertX;       // negative insertion on X axis
    uint32_t insertY;       // negative insertion on Y axis
    uint32_t flip;          // 0: no flip; 1: flip horizontally; 2: flip vertically
    uint32_t subHeaderPtr;  // byte length of data after header
    uint32_t unitWidth;     // N/A
    uint32_t unitHeight;    // N/A
    uint8_t pad[8];         // 8 x '\x00'
};

#define FME_SUBHEADER_FORMAT "%l4 %l4 %l4 %l4 %l4 %c4"
struct FMESubHeader {
    uint32_t sizeX;         // image width
    uint32_t sizeY;         // image height
    uint32_t compressed;    // 0: not compressed; 1: RLE0 compressed;
    uint32_t dataSize;      // byte length of data after header
    uint32_t columnOffset;  // N/A
    uint8_t pad[4];         // 4 x '\x00'
};

struct FMEFile{
    FMEHeader* header;
    FMESubHeader* subHeader;
    uint8_t* data;
};

#endif
