#ifndef ODF_RES_PAL_H
#define ODF_RES_PAL_H

#include "odf/res/types/pal.h"
#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"


// Return type: Optional(Palette)
OptionalPtr* palOpen(InMemoryFile* file);

ucvec3 palGetColor(Palette* palette, uint8_t index);

void palUnindex(Palette* palette, ucvec4* buffer, bool isTransparent, uint8_t* indexedStream, uint32_t length);

void palClose(Palette* pal);

#endif
