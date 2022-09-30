#ifndef DRIVERS_FME_H
#define DRIVERS_FME_H

#include "types.h"
#include "system/optional.h"
#include "inmemoryfile.h"

#define FME_FLIP_NONE 0
#define FME_FLIP_HORIZONTALLY 1
#define FME_FLIP_VERTICALLY 2

#define FME_COMPRESS_NONE 0
#define FME_COMPRESS_RLE0 1

typedef struct {
    uint32 insertX;       // negative insertion on X axis
    uint32 insertY;       // negative insertion on Y axis
    uint32 flip;          // 0: no flip; 1: flip horizontally; 2: flip vertically
    uint32 subHeaderPtr;  // byte length of data after header
    uint32 unitWidth;     // N/A
    uint32 unitHeight;    // N/A
    uint8 pad[8];         // 8 x '\x00'
} FMEHeader;

typedef struct {
    uint32 sizeX;         // image width
    uint32 sizeY;         // image height
    uint32 compressed;    // 0: not compressed; 1: RLE0 compressed;
    uint32 dataSize;      // byte length of data after header
    uint32 columnOffset;  // N/A
    uint8 pad[4];         // 4 x '\x00'
} FMESubHeader;

typedef struct {
    FMEHeader* header;
    FMESubHeader* subHeader;
    uint8* data;
} FMEFile;

OptionalPtr* fmeOpenInMemoryFile(InMemoryFile*);
void fmeClose(FMEFile*);

#endif
