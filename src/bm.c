#include <stdio.h>
#include <stdlib.h>

#include "../headers/types.h"
#include "../headers/bm.h"
#include "../headers/pal.h"

#define BM_NORMAL 0x36
#define BM_TRANSPARENT 0x3e
#define BM_WEAPON 0x08

typedef struct {
    char magic[4];
    uint16 sizeX;
    uint16 sizeY;
    uint16 idemX;
    uint16 idemY;
    uint8 transparent;
    uint8 logSizeY;
    uint16 compressed;
    uint32 dataSize;
    char pad[12];
} BMHeader;

struct BMFile {
    BMHeader* header;
    char* data;
};

BMFile* bmOpenFile(char* file) {
    BMFile* bmFile = malloc(sizeof(BMFile));
    BMHeader* bmHeader = malloc(sizeof(BMHeader));

    bmFile->header = bmHeader;

    FILE* stream = fopen(file, "rb");

    fread(bmHeader, sizeof(BMHeader), 1, stream);

    bmFile->data = malloc(bmHeader->dataSize);
    fread(bmFile->data, bmHeader->dataSize, 1, stream);

    return bmFile;
}

void bmCloseFile(BMFile* bmFile) {
    free(bmFile->header);
    free(bmFile->data);
    free(bmFile);
}

uint32 bmGlBindImageTexture(BMFile* bmFile, Palette* palette) {
    return 0;
}

void bmPrintFile(BMFile* bmFile) {
    BMHeader* bmHeader = bmFile->header;
    uint32* magic = (uint32*) &(bmHeader->magic);

    printf("magic: 0x%x\n", *magic);
    printf("sizeX: %d\n", bmHeader->sizeX);
    printf("sizeY: %d\n", bmHeader->sizeY);
    printf("idemX: %d\n", bmHeader->idemX);
    printf("idemY: %d\n", bmHeader->idemY);
    printf("transparent: 0x%x\n", bmHeader->transparent);
    printf("logSizeY: %d\n", bmHeader->logSizeY);
    printf("compressed: %d\n", bmHeader->compressed);
    printf("dataSize: %d\n", bmHeader->dataSize);
}
