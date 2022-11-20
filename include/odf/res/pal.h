#ifndef ODF_RES_PAL_H
#define ODF_RES_PAL_H

#include "odf/res/types/pal.h"
#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"


// Return type: Optional(Palette)
OptionalPtr* palOpen(InMemoryFile* file);

ucvec3 palGetColor(Palette* palette, uint8 index);

void palClose(Palette* pal);

#endif
