#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "odf/res/bm.h"
#include "odf/res/types/bm_def.h"
#include "odf/res/pal.h"

#include "odf/sys/file.h"
#include "odf/res/rle.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"

bool bmIsTransparent(BMFile* bmFile);

static uint8_t* bmDecompress(BMFile* bmFile);
static Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8_t* data,  Palette* palette);


BMFile* bmOpenFile(char* file) {
    FILE* stream = fopen(file, "rb");

    OptionalPtr* optionalBmHeader = fileReadStruct(stream, BM_HEADER_FORMAT);
    if(optionalIsEmpty(optionalBmHeader)) return NULL;

    BMHeader* bmHeader = optionalGet(optionalBmHeader);
    memoryTag(bmHeader, "odf/res/bm/BmHeader");

    OptionalPtr* optData = fileReadBytes(stream, bmHeader->dataSize);
    if(optionalIsEmpty(optData)) return NULL;
    uint8_t* data = optionalGet(optData);
    memoryTag(data, "odf/res/bm/BMFile/data");

    fclose(stream);

    BMFile* bmFile = memoryAllocateWithTag(sizeof(BMFile), "odf/res/bm/BMFile");
    bmFile->header = bmHeader;
    bmFile->data = data;

    return bmFile;
}

BMFile* bmOpenInMemoryFile(InMemoryFile* file) {
    OptionalPtr* optionalBmHeader = inMemFileReadStruct(file, BM_HEADER_FORMAT);
    if(optionalIsEmpty(optionalBmHeader)) return NULL;

    BMHeader* bmHeader = optionalGet(optionalBmHeader);

    OptionalPtr* optionalData = inMemFileRead(file, bmHeader->dataSize);
    if(optionalIsEmpty(optionalData)) return NULL;

    BMFile* bmFile = memoryAllocateWithTag(sizeof(BMFile), "odf/res/bm/BMFile");

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
    uint32_t compressed = bmFile->header->compressed;
    if(compressed == BM_COMPRESSION_NONE) {
        return bmCreateImageDecompressed(bmFile, bmFile->data, palette);
    }

    uint8_t* data = bmDecompress(bmFile);
    Image8Bit* image = bmCreateImageDecompressed(bmFile, data, palette);
    memoryRelease(data);

    return image;
}

uint8_t* bmDecompress(BMFile* bmFile) {
    int width = bmFile->header->sizeX;
    int height = bmFile->header->sizeY;
    int length = bmFile->header->dataSize;

    uint32_t compressed = bmFile->header->compressed;
    if(compressed == BM_COMPRESSION_RLE0) {
        return rle0Decompress(bmFile->data, length, width, height);
    } else {
        return rle1Decompress(bmFile->data, length, width, height);
    }
}

static uint8_t* rotcc90(uint8_t* image, int w,int h);
Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8_t* data,  Palette* palette) {
    size_t w = bmFile->header->sizeX;
    size_t h = bmFile->header->sizeY;
    Image8Bit* img = img8bCreate2D(w, h, 4);
    ucvec4* texture = (ucvec4*) img->data;
    uint8_t* rotatedImage = rotcc90(data, w, h);
    palUnindex(palette, texture, bmIsTransparent(bmFile), rotatedImage, w*h);
    memoryRelease(rotatedImage);

    return img;
}

static uint8_t* rotcc90(uint8_t* image, int w,int h) {
    uint8_t* rotatedImage = memoryAllocate(w*h*sizeof(uint8_t));

    for(int i1 = 0; i1 <  w*h; i1++) {
        int x = i1/h;
        int y = i1%h;
        int i2 = x + w*y;

        rotatedImage[i2] = image[i1];
    }

    return rotatedImage;
}

bool bmIsTransparent(BMFile* bmFile) {
    uint8_t transparent = bmFile->header->transparent;

    return (transparent == BM_TRANSPARENT || transparent == BM_WEAPON);
}

void bmPrintFile(BMFile* bmFile) {
    BMHeader* bmHeader = bmFile->header;
    uint32_t* magic = (uint32_t*) &(bmHeader->magic);

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
