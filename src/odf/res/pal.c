#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "odf/res/pal.h"
#include "odf/res/types/pal_def.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

OptionalPtr* palOpenInMemoryFile(InMemoryFile* file){
    uint64 fileSize = inMemFileSize(file);
    if(fileSize != sizeof(Palette)){
        return optionalEmpty(
            "Size of file (%lu) does not match Palette specifications (%lu)\n", fileSize, sizeof(Palette));
    }

    OptionalPtr* optContent = inMemFileRead(file, fileSize);
    if(optionalIsEmpty(optContent))
        return optionalEmpty("odf/res/pal.c:palOpenInMemoryFile - Could not read Palette");
    Palette* content = optionalGet(optContent);
    memoryTag(content, "odf/res/pal/Palette");

    return optionalOf(content);
}


void palClose(Palette* pal) {
    memoryRelease(pal);
}
