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
#include "odf/sys/log.h"
#include "odf/sys/list.h"

bool bmIsTransparent(BMFile* bmFile);

static OptionalOf(ListOf(BMSubFile*)*)* bmReadSubFiles(InMemoryFile* file, BMHeader* header);
static uint8_t* bmDecompress(BMFile* bmFile);
static Image8Bit* bmCreateImageDecompressed(BMFile* bmFile, uint8_t* data,  Palette* palette);
static void bmCloseSubFiles(ListOf(BMSubFile*)* subFiles);


BMFile* bmOpenFile(char* file) {
    FILE* stream = fopen(file, "rb");
    BMFile* bmFile = memoryAllocateWithTag(sizeof(BMFile), "odf/res/bm/BMFile");
    bmFile->header = NULL;
    bmFile->subBMFiles = NULL;
    bmFile->data = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
        BMHeader*, header, fileReadStruct(stream, BM_HEADER_FORMAT),
        {
            fclose(stream);
        },
        NULL
    );
    memoryTag(header, "odf/res/bm/BmHeader");
    bmFile->header = header;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
        uint8_t*, data, fileReadBytes(stream, header->dataSize),
        {
            bmClose(bmFile);
            fclose(stream);
        },
        NULL
    )
    memoryTag(data, "odf/res/bm/BMFile/data");
    bmFile->data = data;

    fclose(stream);

    return bmFile;
}

BMFile* bmOpenInMemoryFile(InMemoryFile* file) {
    logTrace("Assembling BM File");
    BMFile* bmFile = memoryAllocateWithTag(sizeof(BMFile), "odf/res/bm/BMFile");
    bmFile->header = NULL;
    bmFile->subBMFiles = NULL;
    bmFile->frameRate = 0;
    bmFile->data = NULL;

    logTrace("Reading BM header");
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
        BMHeader*, bmHeader, inMemFileReadStruct(file, BM_HEADER_FORMAT),
        {
            logWarn("Could not read BM header");
            bmClose(bmFile);
        },
        NULL
    )
    bmFile->header = bmHeader;

    if(bmHeader->sizeX == 1 && bmHeader->sizeY > 1) {
        bmPrintFile(bmFile);
        logTrace("Reading frameRate");
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
            uint8_t*, frameRate, inMemFileReadStruct(file, "%c1"),
            {
                logWarn("Could not read frame rate");
                bmClose(bmFile);
            },
            NULL
        );
        logTrace("Multiple BM frame rate is %d", *frameRate);
        bmFile->frameRate = *frameRate;
        memoryRelease(frameRate);

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
            ListOf(BMSubFile*)*, innerSubFiles, bmReadSubFiles(file, bmFile->header),
            {
                logWarn("Could not read BM subheaders");
                bmClose(bmFile);
            },
            NULL
        );

        bmFile->subBMFiles = innerSubFiles;
    } else {
        logTrace("Reading BM Data");
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_RETURN(
            uint8_t*, innerData, inMemFileRead(file, bmHeader->dataSize),
            {
                logWarn("Could not read BM data");
                bmClose(bmFile);
            },
            NULL
        )
        bmFile->data = innerData;
    }

    return bmFile;
}


static OptionalOf(ListOf(BMSubFile*)*)* bmReadSubFiles(InMemoryFile* file, BMHeader* header) {
    OPTIONAL_ASSIGN_OR_PASSTROUGH(uint8_t*, byte2, inMemFileReadStruct(file, "%c1"));
    if(*byte2 != 2) {
        logTrace("Byte after framerate is not '\\x02': '%x'", *byte2);
        memoryRelease(byte2);
        return optionalEmpty("Byte after framerate is not '\\x02'");
    }
    memoryRelease(byte2);


    uint16_t subHeaderCount = header->sizeY;

    logTrace("Reading offsets");
    ListOf(uint8_t*)* offsets = listCreate(subHeaderCount);
    for(uint16_t i = 0; i < subHeaderCount; i++) {
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint32_t*, offset, inMemFileReadStruct(file, "%l4"), {
                forEachListItem(uint32_t*, o, offsets, {
                    memoryRelease(o);
                });
                memoryRelease(offsets);
            }
        )
        *offset = *offset + sizeof(BMHeader) + 2L;
        logTrace("Found offset %d", *offset);
        listAppend(offsets, offset);
    }


    logTrace("Reading subheaders");
    ListOf(BMSubFile*)* subFiles = listCreate(header->sizeY);
    forEachListItem(uint32_t*, offset, offsets, {
        memFileSeek(file, *offset, SEEK_SET);

        BMSubFile* subFile = memoryAllocateWithTag(sizeof(BMSubFile), "odf/res/bm:BMSubFile");
        subFile->header = NULL;
        subFile->data = NULL;
        listAppend(subFiles, subFile);

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            BMSubHeader*, subHeader, inMemFileReadStruct(file, BM_SUBHEADER_FORMAT),
            {
                logTrace("Could not read BM sub header at offset %d", *offset);
                bmCloseSubFiles(subFiles);
            }
        );
        memoryTag(subHeader, "odf/res/bm:BMSubHeader");
        subFile->header = subHeader;

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, data, inMemFileRead(file, subHeader->dataSize),
            {
                logTrace("Could not read BM sub data at offset %d", *offset);
                bmCloseSubFiles(subFiles);
            }
        );
        memoryTag(data, "odf/res/bm:BMSubData");
        subFile->data = data;
    });

    forEachListItem(uint32_t*, offset, offsets, {
        memoryRelease(offset);
    })
    listDelete(offsets);

    return optionalOf(subFiles);
}

void bmClose(BMFile* bmFile) {
    logTrace("Releasing BM file %p", bmFile);

    bmCloseSubFiles(bmFile->subBMFiles);
    if(bmFile->header) memoryRelease(bmFile->header);
    if(bmFile->data) memoryRelease(bmFile->data);

    memoryRelease(bmFile);
}

static void bmCloseSubFiles(ListOf(BMSubFile*)* subFiles) {
    if(!subFiles) return;

    forEachListItem(BMSubFile*, subFile, subFiles, {
        logTrace("Releasing BM subfile %p", subFile);

        if(subFile->header) memoryRelease(subFile->header);
        if(subFile->data) memoryRelease(subFile->data);
        memoryRelease(subFile);
    });

    listDelete(subFiles);
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

    logTrace("magic: 0x%x", *magic);
    logTrace("sizeX: %d", bmHeader->sizeX);
    logTrace("sizeY: %d", bmHeader->sizeY);
    logTrace("idemX: %d", bmHeader->idemX);
    logTrace("idemY: %d", bmHeader->idemY);
    logTrace("transparent: 0x%x", bmHeader->transparent);
    logTrace("logSizeY: %d", bmHeader->logSizeY);
    logTrace("compressed: %d", bmHeader->compressed);
    logTrace("dataSize: %d", bmHeader->dataSize);
}
