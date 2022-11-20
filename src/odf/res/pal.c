#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "odf/res/pal.h"
#include "odf/res/types/pal_def.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"


static ucvec3 normalizedPaletteColor(ucvec3* color);

OptionalPtr* palOpen(InMemoryFile* file){
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

ucvec3 palGetColor(Palette* palette, uint8 index){
    ucvec3* paletteColor = palette->colors + index;
    return normalizedPaletteColor(paletteColor);
}

void palClose(Palette* pal) {
    memoryRelease(pal);
}

static ucvec3 normalizedPaletteColor(ucvec3* color) {
    float scale = (255.0 / 63.0);
    ucvec3 vec = {
        .r = round(scale * color->r),
        .g = round(scale * color->g),
        .b = round(scale * color->b),
    };

    return vec;
}
