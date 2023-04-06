#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "odf/res/bm.h"
#include "odf/res/types/bm_def.h"
#include "odf/res/pal.h"

#include "odf/res/rle.h"
#include "odf/sys/file.h"
#include "odf/sys/strings.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/log.h"
#include "odf/sys/list.h"

bool bmIsTransparent(BMFile* bmFile);

static OptionalOf(ListOf(BMSubFile*)*)* bmReadSubFiles(InMemoryFile* file, BMHeader* header);
static Image8Bit* bmCreateRawImage(uint8_t* data, uint32_t dataSize, uint16_t width, uint16_t height, uint16_t compressed, uint8_t transparent, Palette* palette);
static Image8Bit* bmCreateImageDecompressed(uint8_t* data, uint32_t width, uint32_t height, uint8_t transparent, Palette* palette);
static uint8_t* bmDecomress(uint8_t* data, uint32_t dataSize, uint16_t width, uint16_t height, uint16_t compressed);
static void bmCloseSubFiles(ListOf(BMSubFile*)* subFiles);
static bool _isTransparent(uint8_t transparent);
static Image8Bit* bmCreateSubImage(BMFile* bmFile, BMSubFile* subFile, Palette* palette);
static void bmPrintHeader(BMHeader* header);
static void bmPrintSubHeader(BMSubHeader* subHeader);
static void bmCorrectHeader(BMHeader* header, InMemoryFile* file);
static bool bmIsMultiple(BMHeader* header, InMemoryFile* file);
static bool bmHasEndiannessIssues(BMHeader* header, InMemoryFile* file);
static OptionalOf(BMHeader*)* bmReadHeader(InMemoryFile* file);



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

OptionalOf(BMFile*)* bmOpenInMemoryFile(InMemoryFile* file) {
    logTrace("Assembling BM File");
    BMFile* bmFile = memoryAllocateWithTag(sizeof(BMFile), "odf/res/bm/BMFile");
    bmFile->header = NULL;
    bmFile->subBMFiles = NULL;
    bmFile->frameRate = 0;
    bmFile->data = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        BMHeader*, bmHeader, bmReadHeader(file),
        {
            bmClose(bmFile);
        }
    )

    bmFile->header = bmHeader;
    logTrace("Assigned BM Header:");
    bmPrintHeader(bmHeader);


    if(bmIsMultiple(bmHeader, file)) {
        logTrace("Reading frameRate");
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, frameRate, inMemFileReadStruct(file, "%c1"),
            {
                logWarn("Could not read frame rate");
                bmClose(bmFile);
            }
        );
        logTrace("Multiple BM frame rate is %d", *frameRate);
        bmFile->frameRate = *frameRate;
        memoryRelease(frameRate);

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            ListOf(BMSubFile*)*, innerSubFiles, bmReadSubFiles(file, bmFile->header),
            {
                logWarn("Could not read BM subheaders");
                bmClose(bmFile);
            }
        );

        bmFile->subBMFiles = innerSubFiles;
    } else if (bmFile->header->compressed == BM_COMPRESSION_NONE) {
        logTrace("Reading BM Data");
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, innerData, inMemFileRead(file, bmHeader->dataSize),
            {
                logWarn("Could not read BM data");
                bmClose(bmFile);
            }
        )
        bmFile->data = innerData;
    } else {
        logTrace("Reading Compressed BM Data");
        RLEConfig config = {
            .type = bmFile->header->compressed == BM_COMPRESSION_RLE0 ?
                RLE0 : RLE1,
            .fileOffset = getBlockLenghtFromFormat(BM_HEADER_FORMAT),
            .offsetCount = bmFile->header->sizeX,
            .sequenceSize = bmFile->header->sizeY,
        };

        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            uint8_t*, innerData, rleReadFile(file, &config),
            {
                logWarn("Could not read BM data");
                bmClose(bmFile);
            }
        );
        bmFile->data = innerData;
    }

    return optionalOf(bmFile);
}

static OptionalOf(BMHeader*)* bmReadHeader(InMemoryFile* file) {
    logTrace("Reading BM header");
    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        BMHeader*, header, inMemFileReadStruct(file, BM_HEADER_FORMAT),
        {logWarn("Could not read BM header");}
    )
    logTrace("Read header:");
    bmPrintHeader(header);
    if(memcmp(header->magic, "BM \x1e", 4) != 0) {
        return optionalEmpty("File is not a proper .BM file. Magic value mismatch");
    }
    bmCorrectHeader(header, file);

    return optionalOf(header);
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
        bmPrintSubHeader(subHeader);

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

ListOf(Image8Bit*)* bmCreateImages(BMFile* bmFile, Palette* palette) {
    BMHeader* header = bmFile->header;

    if(bmFile->subBMFiles != NULL) {
        ListOf(BMSubFile*)* subFiles = bmFile->subBMFiles;
        ListOf(Image8Bit*)* images = listCreate(listSize(subFiles));

        forEachListItem(BMSubFile*, subFile, subFiles, {
            listAppend(images, bmCreateSubImage(bmFile, subFile, palette));
        });

        return images;
    } else {
        return listOf(
            bmCreateImage(bmFile, palette)
        );
    }
}

Image8Bit* bmCreateImage(BMFile* bmFile, Palette* palette) {
    BMHeader* header = bmFile->header;

    return bmCreateRawImage(
        bmFile->data,
        header->dataSize,
        header->sizeX,
        header->sizeY,
        header->compressed,
        header->transparent,
        palette
    );
}

static Image8Bit* bmCreateSubImage(BMFile* bmFile, BMSubFile* subFile, Palette* palette) {
    BMHeader* header = bmFile->header;
    BMSubHeader* subHeader = subFile->header;

    return bmCreateRawImage(
        subFile->data,
        subHeader->dataSize,
        subHeader->sizeX,
        subHeader->sizeY,
        header->compressed,
        subHeader->transparent,
        palette
    );
}

static Image8Bit* bmCreateRawImage(uint8_t* data, uint32_t dataSize, uint16_t width, uint16_t height, uint16_t compressed, uint8_t transparent, Palette* palette) {
    if(compressed == BM_COMPRESSION_NONE) {
        return bmCreateImageDecompressed(data, width, height, transparent, palette);
    }

    uint8_t* decompressedData = bmDecomress(data, dataSize, width, height, compressed);
    Image8Bit* image = bmCreateImageDecompressed(decompressedData, width, height, transparent, palette);
    memoryRelease(decompressedData);

    return image;
}

static uint8_t* bmDecomress(uint8_t* data, uint32_t dataSize, uint16_t width, uint16_t height, uint16_t compressed) {
    if(compressed == BM_COMPRESSION_RLE0) {
        return rle0Decompress(data, dataSize, width, height);
    } else {
        return rle1Decompress(data, dataSize, width, height);
    }
}


static uint8_t* rotcc90(uint8_t* image, int w,int h);

static Image8Bit* bmCreateImageDecompressed(uint8_t* data, uint32_t width, uint32_t height, uint8_t transparent, Palette* palette) {
    uint32_t length = width * height;
    Image8Bit* img = img8bCreate2D(width, height, 4);
    ucvec4* texture = (ucvec4*) img->data;
    palUnindex(palette, texture, _isTransparent(transparent), data, length);

    return img;
}

bool bmIsTransparent(BMFile* bmFile) {
    return _isTransparent(bmFile->header->transparent);
}

static bool _isTransparent(uint8_t transparent) {
    return (transparent == BM_TRANSPARENT || transparent == BM_WEAPON);
}

static void bmCorrectHeader(BMHeader* header, InMemoryFile* file) {
    if(bmHasEndiannessIssues(header, file)) {
        logDebug("BM File has incorrect endianness");
        reverseEndianness(&(header->sizeX), sizeof(uint16_t));
        reverseEndianness(&(header->sizeY), sizeof(uint16_t));
        reverseEndianness(&(header->idemX), sizeof(uint16_t));
        reverseEndianness(&(header->idemY), sizeof(uint16_t));
        reverseEndianness(&(header->dataSize), sizeof(uint32_t));
    }
    if(header->compressed == 0 && header->dataSize == 0) {
        header->dataSize = (uint32_t) header->sizeX * (uint32_t) header->sizeY;
    }
}

static bool bmHasEndiannessIssues(BMHeader* header, InMemoryFile* file) {
    return bmIsMultiple(header, file) &&
        header->compressed == 0 &&
        header->dataSize != header->sizeX * header->sizeY;
}

static bool bmIsMultiple(BMHeader* header, InMemoryFile* file) {
    size_t size = inMemFileSize(file);
    uint16_t w = header->sizeX;
    uint16_t h = header->sizeY;
    return w == 1 &&
        h > 1 &&
        size != w*h + sizeof(BMHeader);
}

void bmPrintFile(BMFile* bmFile) {
    bmPrintHeader(bmFile->header);
    forEachListItem(BMSubFile*, subFile, bmFile->subBMFiles, {
        bmPrintSubHeader(subFile->header);
    });
}

void bmPrintHeader(BMHeader* header) {
    uint32_t* magic = (uint32_t*) &(header->magic);

    logTrace("magic: 0x%x", *magic);
    logTrace("sizeX: %d", header->sizeX);
    logTrace("sizeY: %d", header->sizeY);
    logTrace("idemX: %d", header->idemX);
    logTrace("idemY: %d", header->idemY);
    logTrace("transparent: 0x%x", header->transparent);
    logTrace("logSizeY: %d", header->logSizeY);
    logTrace("compressed: %d", header->compressed);
    logTrace("dataSize: %d", header->dataSize);
}

void bmPrintSubHeader(BMSubHeader* subHeader) {
    logTrace("sizeX: %d", subHeader->sizeX);
    logTrace("sizeY: %d", subHeader->sizeY);
    logTrace("idemX: %d", subHeader->idemX);
    logTrace("idemY: %d", subHeader->idemY);
    logTrace("transparent: 0x%x", subHeader->transparent);
    logTrace("logSizeY: %d", subHeader->logSizeY);
    logTrace("dataSize: %d", subHeader->dataSize);
}
