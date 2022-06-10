#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pal.h"
#include "inmemoryfile.h"

Palette* palOpenInMemoryFile(InMemoryFile* file){
    if(file->length != sizeof(Palette)){
        printf("Size of file (%d) does not match Palette specifications (%lu)\n", file->length, sizeof(Palette));
        return NULL;
    }

    Palette* palette = malloc(sizeof(Palette));
    if(palette == NULL) {
        printf("Out of memory");
        return NULL;
    }
    memcpy(palette, file->content, sizeof(Palette));

    return palette;
}
void palClose(Palette* pal) {
    free(pal);
}
