#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "odf/sys/file.h"
#include "odf/res/bm.h"
#include "odf/res/rle.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"

bool bmIsTransparent(BMFile* bmFile, int colorIndex);

uint8* bmDecompress(BMFile* bmFile);
Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8* data,  Palette* palette);
int bmGetNormalizedPixelIndex(int w, int h, int i);
void ucvec3Copy(ucvec3* dest, ucvec3* src);

const char* BM_HEADER_FORMAT =
//  magic | sizeX | sizeY | |idemX | idemY | trans | logSizeY | compressed | dataSize | padding
    "%c4     %l2     %l2      %l2     %l2     %c1       %c1         %l2        %l4       %c12";

BMFile* bmOpenFile(char* file) {
    FILE* stream = fopen(file, "rb");

    OptionalPtr* optionalBmHeader = fileReadStruct(stream, BM_HEADER_FORMAT);
    if(optionalIsEmpty(optionalBmHeader)) return NULL;

    BMHeader* bmHeader = optionalGet(optionalBmHeader);

    OptionalPtr* data = fileReadBytes(stream, bmHeader->dataSize);
    if(optionalIsEmpty(data)) return NULL;

    fclose(stream);

    BMFile* bmFile = memoryAllocate(sizeof(BMFile));
    bmFile->header = bmHeader;
    bmFile->data = optionalGet(data);

    return bmFile;
}

BMFile* bmOpenInMemoryFile(InMemoryFile* file) {
    OptionalPtr* optionalBmHeader = inMemFileReadStruct(file, BM_HEADER_FORMAT);
    if(optionalIsEmpty(optionalBmHeader)) return NULL;

    BMHeader* bmHeader = optionalGet(optionalBmHeader);

    OptionalPtr* optionalData = inMemFileRead(file, bmHeader->dataSize);
    if(optionalIsEmpty(optionalData)) return NULL;

    BMFile* bmFile = memoryAllocate(sizeof(BMFile));

    bmFile->header = bmHeader;
    bmFile->data = optionalGet(optionalData);

    return bmFile;
}

void bmClose(BMFile* bmFile) {
    memoryRelease(bmFile->header);
    memoryRelease(bmFile->data);
    memoryRelease(bmFile);
}



Image8Bit* bmCreateImage(BMFile* bmFile, Palette* palette) {
    uint32 compressed = bmFile->header->compressed;
    if(compressed == BM_COMPRESSION_NONE) {
        return bmCreateImageDecompressed(bmFile, bmFile->data, palette);
    }

    uint8* data = bmDecompress(bmFile);
    Image8Bit* image = bmCreateImageDecompressed(bmFile, data, palette);
    memoryRelease(data);

    return image;
}

uint8* bmDecompress(BMFile* bmFile) {
    int width = bmFile->header->sizeX;
    int height = bmFile->header->sizeY;
    int length = bmFile->header->dataSize;

    uint32 compressed = bmFile->header->compressed;
    if(compressed == BM_COMPRESSION_RLE0) {
        return rle0Decompress(bmFile->data, length, width, height);
    } else {
        return rle1Decompress(bmFile->data, length, width, height);
    }
}

Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8* data,  Palette* palette) {
    int w = bmFile->header->sizeX;
    int h = bmFile->header->sizeY;
    Image8Bit* img = img8bCreate2D(w, h, 4);
    ucvec4* texture = (ucvec4*) img->data;

    for (int i =0; i < w*h; i++) {
        int pxi = bmGetNormalizedPixelIndex(w, h, i);

        unsigned char colorIndex = data[i];
        ucvec3* paletteColor = palette->colors + colorIndex;
        ucvec4* pixel = texture + pxi;

        ucvec3Copy((ucvec3*) pixel, paletteColor);
        if(bmIsTransparent(bmFile, colorIndex)){
            pixel->a = 0;
        } else {
            pixel->a = 255;
        }
    }

    return img;
}

int bmGetNormalizedPixelIndex(int w, int h, int i) {
    // BM contains pixels from bottomleft to topleft, then right
    int x = i/h;
    int y = i%h;
    return x + w*y;
}

void ucvec3Copy(ucvec3* dest, ucvec3* src) {
    dest->r = src->r;
    dest->g = src->g;
    dest->b = src->b;
}

bool bmIsTransparent(BMFile* bmFile, int colorIndex) {
    uint8 transparent = bmFile->header->transparent;
    return (transparent == BM_TRANSPARENT || transparent == BM_WEAPON)
        && colorIndex == '\x00';
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
