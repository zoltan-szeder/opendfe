#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "odf/res/pal.h"
#include "odf/res/types/pal_def.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

Palette* palOpenInMemoryFile(InMemoryFile* file){
    uint64 fileSize = inMemFileSize(file);
    if(fileSize != sizeof(Palette)){
        printf("Size of file (%lu) does not match Palette specifications (%lu)\n", fileSize, sizeof(Palette));
        return NULL;
    }

    OptionalPtr* optContent = inMemFileRead(file, fileSize);
    if(optionalIsEmpty(optContent)) return NULL;
    Palette* content = optionalGet(optContent);
    memoryTag(content, "odf/res/pal/Palette");

    return content;
}
void palClose(Palette* pal) {
    memoryRelease(pal);
}
