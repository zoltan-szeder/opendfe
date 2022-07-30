#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "file.h"
#include "drivers/bm.h"
#include "drivers/rle.h"
#include "system/memory.h"

bool bmIsTransparent(BMFile* bmFile, int colorIndex);

uint8* bmDecompress(BMFile* bmFile);
Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8* data,  Palette* palette);
int bmGetNormalizedPixelIndex(int w, int h, int i);
void ucvec3Copy(ucvec3* dest, ucvec3* src);

OptionalPtr* bmReadHeader(FILE* stream) {
    OptionalPtr* magic = fileReadBytes(stream, 4);
    if(optionalIsEmpty(magic)) return NULL;
    OptionalUInt16* sizeX = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(sizeX)) return NULL;
    OptionalUInt16* sizeY = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(sizeY)) return NULL;
    OptionalUInt16* idemX = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(idemX)) return NULL;
    OptionalUInt16* idemY = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(idemY)) return NULL;
    OptionalUInt8* transparent = fileReadLittleEndianUInt8(stream);
    if(optionalIsEmpty(transparent)) return NULL;
    OptionalUInt8* logSizeY = fileReadLittleEndianUInt8(stream);
    if(optionalIsEmpty(logSizeY)) return NULL;
    OptionalUInt16* compressed = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(compressed)) return NULL;
    OptionalUInt32* dataSize = fileReadLittleEndianUInt32(stream);
    if(optionalIsEmpty(dataSize)) return NULL;
    OptionalPtr* pad = fileReadBytes(stream, 12);
    if(optionalIsEmpty(pad)) return NULL;

    BMHeader* bmHeader = malloc(sizeof(BMHeader));
    bmHeader->magic = optionalGet(magic);
    bmHeader->sizeX = optionalGetUInt16(sizeX);
    bmHeader->sizeY = optionalGetUInt16(sizeY);
    bmHeader->idemX = optionalGetUInt16(idemX);
    bmHeader->idemY = optionalGetUInt16(idemY);
    bmHeader->transparent = optionalGetUInt8(transparent);
    bmHeader->logSizeY = optionalGetUInt8(logSizeY);
    bmHeader->compressed = optionalGetUInt16(compressed);
    bmHeader->dataSize = optionalGetUInt32(dataSize);
    bmHeader->pad = optionalGet(pad);

}

BMFile* bmOpenFile(char* file) {

    FILE* stream = fopen(file, "rb");

    OptionalPtr* magic = fileReadBytes(stream, 4);
    if(optionalIsEmpty(magic)) return NULL;
    OptionalUInt16* sizeX = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(sizeX)) return NULL;
    OptionalUInt16* sizeY = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(sizeY)) return NULL;
    OptionalUInt16* idemX = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(idemX)) return NULL;
    OptionalUInt16* idemY = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(idemY)) return NULL;
    OptionalUInt8* transparent = fileReadLittleEndianUInt8(stream);
    if(optionalIsEmpty(transparent)) return NULL;
    OptionalUInt8* logSizeY = fileReadLittleEndianUInt8(stream);
    if(optionalIsEmpty(logSizeY)) return NULL;
    OptionalUInt16* compressed = fileReadLittleEndianUInt16(stream);
    if(optionalIsEmpty(compressed)) return NULL;
    OptionalUInt32* dataSize = fileReadLittleEndianUInt32(stream);
    if(optionalIsEmpty(dataSize)) return NULL;
    OptionalPtr* pad = fileReadBytes(stream, 12);
    if(optionalIsEmpty(pad)) return NULL;

    BMHeader* bmHeader = malloc(sizeof(BMHeader));
    bmHeader->magic = optionalGet(magic);
    bmHeader->sizeX = optionalGetUInt16(sizeX);
    bmHeader->sizeY = optionalGetUInt16(sizeY);
    bmHeader->idemX = optionalGetUInt16(idemX);
    bmHeader->idemY = optionalGetUInt16(idemY);
    bmHeader->transparent = optionalGetUInt8(transparent);
    bmHeader->logSizeY = optionalGetUInt8(logSizeY);
    bmHeader->compressed = optionalGetUInt16(compressed);
    bmHeader->dataSize = optionalGetUInt32(dataSize);
    bmHeader->pad = optionalGet(pad);

    OptionalPtr* data = fileReadBytes(stream, bmHeader->dataSize);
    if(optionalIsEmpty(data)) return NULL;

    fclose(stream);

    BMFile* bmFile = malloc(sizeof(BMFile));
    bmFile->header = bmHeader;
    bmFile->data = optionalGet(data);

    return bmFile;
}

BMFile* bmOpenInMemoryFile(InMemoryFile* file) {
    OptionalPtr* magic = inMemFileRead(file, 4);
    if(optionalIsEmpty(magic)) return NULL;
    OptionalUInt16* sizeX = inMemFileReadLittleEndianUInt16(file);
    if(optionalIsEmpty(sizeX)) return NULL;
    OptionalUInt16* sizeY = inMemFileReadLittleEndianUInt16(file);
    if(optionalIsEmpty(sizeY)) return NULL;
    OptionalUInt16* idemX = inMemFileReadLittleEndianUInt16(file);
    if(optionalIsEmpty(idemX)) return NULL;
    OptionalUInt16* idemY = inMemFileReadLittleEndianUInt16(file);
    if(optionalIsEmpty(idemY)) return NULL;
    OptionalUInt8* transparent = inMemFileReadLittleEndianUInt8(file);
    if(optionalIsEmpty(transparent)) return NULL;
    OptionalUInt8* logSizeY = inMemFileReadLittleEndianUInt8(file);
    if(optionalIsEmpty(logSizeY)) return NULL;
    OptionalUInt16* compressed = inMemFileReadLittleEndianUInt16(file);
    if(optionalIsEmpty(compressed)) return NULL;
    OptionalUInt32* dataSize = inMemFileReadLittleEndianUInt32(file);
    if(optionalIsEmpty(dataSize)) return NULL;
    OptionalPtr* pad = inMemFileRead(file, 12);
    if(optionalIsEmpty(pad)) return NULL;

    OptionalPtr* optionalBmHeader = memoryAllocate(sizeof(BMHeader));
    if(optionalIsEmpty(optionalBmHeader)) return NULL;

    BMHeader* bmHeader = optionalGet(optionalBmHeader);
    bmHeader->magic = optionalGet(magic);
    bmHeader->sizeX = optionalGetUInt16(sizeX);
    bmHeader->sizeY = optionalGetUInt16(sizeY);
    bmHeader->idemX = optionalGetUInt16(idemX);
    bmHeader->idemY = optionalGetUInt16(idemY);
    bmHeader->transparent = optionalGetUInt8(transparent);
    bmHeader->logSizeY = optionalGetUInt8(logSizeY);
    bmHeader->compressed = optionalGetUInt16(compressed);
    bmHeader->dataSize = optionalGetUInt32(dataSize);
    bmHeader->pad = optionalGet(pad);

    OptionalPtr* optionalData = inMemFileRead(file, bmHeader->dataSize);
    if(optionalIsEmpty(optionalData)) return NULL;

    OptionalPtr* optionalBmFile = memoryAllocate(sizeof(BMFile));
    if(optionalIsEmpty(optionalBmFile)) return NULL;
    BMFile* bmFile = optionalGet(optionalBmFile);

    bmFile->header = bmHeader;
    bmFile->data = optionalGet(optionalData);

    return bmFile;
}

void bmClose(BMFile* bmFile) {
    free(bmFile->header->magic);
    free(bmFile->header->pad);
    free(bmFile->header);
    free(bmFile->data);
    free(bmFile);
}



Image8Bit* bmCreateImage(BMFile* bmFile, Palette* palette) {
    uint32 compressed = bmFile->header->compressed;
    if(compressed == BM_COMPRESSION_NONE) {
        return bmCreateImageDecompressed(bmFile, bmFile->data, palette);
    }

    uint8* data = bmDecompress(bmFile);
    Image8Bit* image = bmCreateImageDecompressed(bmFile, data, palette);
    free(data);

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
