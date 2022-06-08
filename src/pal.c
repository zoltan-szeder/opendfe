#include <string.h>
#include <stdlib.h>
#include "pal.h"
#include "inmemoryfile.h"

Palette* palOpenInMemoryFile(InMemoryFile* file){
    if(file->length != sizeof(Palette)){
        return NULL;
    }

    Palette* palette = malloc(sizeof(Palette));
    memcpy(palette, file->content, sizeof(Palette));

    return palette;
}
void palClose(Palette* pal) {
    free(pal);
}
