#ifndef ODF_RES_PAL_H
#define ODF_RES_PAL_H

#include "odf/res/types/pal.h"
#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"


// Return type: Optional(Palette)
OptionalPtr* palOpen(InMemoryFile* file);

ucvec3 palGetColor(Palette* palette, uint8 index);

void palUnindex(Palette* palette, ucvec4* buffer, bool isTransparent, uint8* indexedStream, uint32 length);

void palClose(Palette* pal);

#endif
