#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "odf/res/pal.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/memory.h"

Palette* palOpenInMemoryFile(InMemoryFile* file){
    uint64 fileSize = inMemFileSize(file);
    if(fileSize != sizeof(Palette)){
        printf("Size of file (%lu) does not match Palette specifications (%lu)\n", fileSize, sizeof(Palette));
        return NULL;
    }

    OptionalPtr* optContent = inMemFileRead(file, fileSize);
    if(optionalIsEmpty(optContent)) return NULL;

    return optionalGet(optContent);
}
void palClose(Palette* pal) {
    memoryRelease(pal);
}
