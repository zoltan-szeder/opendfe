#ifndef DRIVERS_PAL_H
#define DRIVERS_PAL_H

#include "math/vec.h"
#include "inmemoryfile.h"

typedef struct {
    ucvec3 colors[256];
} Palette;

Palette* palOpenInMemoryFile(InMemoryFile* file);
void palClose(Palette* pal);

#endif
