#ifndef ODF_RES_PAL_H
#define ODF_RES_PAL_H

#include "odf/res/types/pal.h"
#include "odf/sys/types/inmemoryfile.h"


Palette* palOpenInMemoryFile(InMemoryFile* file);
void palClose(Palette* pal);

#endif
