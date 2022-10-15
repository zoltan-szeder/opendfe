#ifndef ODF_RES_TYPES_FME_DEF_H
#define ODF_RES_TYPES_FME_DEF_H

#include "odf/res/types/fme.h"
#include "odf/sys/types.h"

#define FME_FLIP_NONE 0
#define FME_FLIP_HORIZONTALLY 1
#define FME_FLIP_VERTICALLY 2

#define FME_COMPRESS_NONE 0
#define FME_COMPRESS_RLE0 1


                        // insX insY flip head2 w   h   pad
#define FME_HEADER_FORMAT "%l4  %l4  %l4  %l4   %l4 %l4 %c8"
                          // sizX  sizY comp dSize cOffset pad
#define FME_SUBHEADER_FORMAT "%l4  %l4  %l4  %l4   %l4     %c4"

struct FMEHeader {
    uint32 insertX;       // negative insertion on X axis
    uint32 insertY;       // negative insertion on Y axis
    uint32 flip;          // 0: no flip; 1: flip horizontally; 2: flip vertically
    uint32 subHeaderPtr;  // byte length of data after header
    uint32 unitWidth;     // N/A
    uint32 unitHeight;    // N/A
    uint8 pad[8];         // 8 x '\x00'
};

struct FMESubHeader {
    uint32 sizeX;         // image width
    uint32 sizeY;         // image height
    uint32 compressed;    // 0: not compressed; 1: RLE0 compressed;
    uint32 dataSize;      // byte length of data after header
    uint32 columnOffset;  // N/A
    uint8 pad[4];         // 4 x '\x00'
};

struct FMEFile{
    FMEHeader* header;
    FMESubHeader* subHeader;
    uint8* data;
};

#endif
