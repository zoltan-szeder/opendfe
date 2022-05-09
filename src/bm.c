#include "../headers/types.h"

#define BM_NORMAL 0x36
#define BM_TRANSPARENT 0x3e
#define BM_WEAPON 0x08

struct BMFile {
    char* magic;
    uint16 sizeX;
    uint16 sizeY;
    uint16 idemX;
    uint16 idemY;
    uint8 transparent;
    uint8 logSizeY;
    uint16 compressed;
    uint32 dataSize;
    char* pad;
};
