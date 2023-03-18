#include "odf/res/fme.h"
#include "odf/res/types/fme_def.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"
#include "odf/sys/strings.h"
#include "odf/res/rle.h"

static OptionalPtr* fmeReadSubHeader(InMemoryFile* file);
static OptionalPtr* fmeReadData(InMemoryFile* file, FMESubHeader* subHeader);

OptionalPtr* fmeOpenInMemoryFile(InMemoryFile* file) {
    FMEFile* fmeFile = memoryAllocateWithTag(sizeof(FMEFile), "odf/res/fme/FMEFile");
    fmeFile->header = NULL;
    fmeFile->subHeader = NULL;
    fmeFile->data = NULL;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        FMEHeader*, header, inMemFileReadStruct(file, FME_HEADER_FORMAT),
        {fmeClose(fmeFile);}
    );
    memoryTag(header, "odf/res/fme/FMEFileHeader");
    fmeFile->header = header;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        FMESubHeader*, subHeader, fmeReadSubHeader(file),
        {fmeClose(fmeFile);}
    );
    fmeFile->subHeader = subHeader;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        uint8_t*, data, fmeReadData(file, subHeader),
        {fmeClose(fmeFile);}
    );
    fmeFile->data = data;

    return optionalOf(fmeFile);
}

static OptionalPtr* fmeReadSubHeader(InMemoryFile* file) {
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        FMESubHeader*, subHeader, inMemFileReadStruct(file, FME_SUBHEADER_FORMAT)
    );
    memoryTag(subHeader, "odf/res/fme/FMESubHeader");

    if(subHeader->compressed == FME_COMPRESS_NONE) {
        uint32_t sizeX = subHeader->sizeX;
        uint32_t sizeY = subHeader->sizeY;
        uint32_t subHeaderSize = getBlockLenghtFromFormat(FME_SUBHEADER_FORMAT);

        subHeader->dataSize = sizeX * sizeY + subHeaderSize;
    }

    return optionalOf(subHeader);
}

static OptionalPtr* fmeReadData(InMemoryFile* file, FMESubHeader* subHeader) {
    size_t subHeaderSize = getBlockLenghtFromFormat(FME_SUBHEADER_FORMAT);

    size_t dataSize = subHeader->dataSize - subHeaderSize;
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        uint8_t*, data, inMemFileRead(file, dataSize)
    );
    memoryTag(data, "odf/res/fme/FMEFileData");

    if(subHeader->compressed == FME_COMPRESS_NONE)
        return optionalOf(data);

    uint8_t* decompressedData = rle0Decompress(
                data, dataSize, subHeader->sizeX, subHeader->sizeY
    );
    memoryRelease(data);

    return optionalOf(decompressedData);

}

void fmeClose(FMEFile* fmeFile) {
    if(!fmeFile) return;

    if(fmeFile->header) memoryRelease(fmeFile->header);
    if(fmeFile->subHeader) memoryRelease(fmeFile->subHeader);
    if(fmeFile->data) memoryRelease(fmeFile->data);

    memoryRelease(fmeFile);
}
