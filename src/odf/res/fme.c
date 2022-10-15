#include "odf/res/fme.h"
#include "odf/res/types/fme_def.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"


OptionalPtr* fmeOpenInMemoryFile(InMemoryFile* file) {
    OptionalPtr* optHeader = inMemFileReadStruct(file, FME_HEADER_FORMAT);
    if(optionalIsEmpty(optHeader)) return optHeader;
    FMEHeader* header = optionalGet(optHeader);
    memoryTag(header, "FMEFileHeader");


    OptionalPtr* optSubHeader = inMemFileReadStruct(file, FME_SUBHEADER_FORMAT);
    if(optionalIsEmpty(optSubHeader)) {
        memoryRelease(header);
        return optSubHeader;
    }
    FMESubHeader* subHeader = optionalGet(optSubHeader);
    memoryTag(subHeader, "FMEFileHeader");

    FMEFile* fmeFile = memoryAllocateWithTag(sizeof(FMEFile), "FMEFile");
    fmeFile->header = header;
    fmeFile->subHeader = subHeader;

    return optionalOf(fmeFile);
}

void fmeClose(FMEFile* fmeFile) {
    memoryRelease(fmeFile->header);
    memoryRelease(fmeFile->subHeader);
    memoryRelease(fmeFile);
}
