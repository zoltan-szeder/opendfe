#ifndef PAL_H
#define PAL_H

#include "types.h"

typedef struct {
    uint8 r;
    uint8 g;
    uint8 b;
} Color;

typedef struct {
    Color colors[256];
} Palette;

Palette* palReadFile(char*);
void palCloseFile(Palette*);


#endif
