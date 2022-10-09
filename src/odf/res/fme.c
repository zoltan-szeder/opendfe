
#include "odf/res/fme.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

                        // insX insY flip head2 w   h   pad
#define FME_HEADER_FORMAT "%l4  %l4  %l4  %l4   %l4 %l4 %c8"
                          // sizX  sizY comp dSize cOffset pad
#define FME_SUBHEADER_FORMAT "%l4  %l4  %l4  %l4   %l4     %c4"

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
