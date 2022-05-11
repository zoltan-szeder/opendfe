#ifndef PAL_H
#define PAL_H

typedef struct {
    uint8 red;
    uint8 green;
    uint8 blue;
} Color;

typedef struct {
    Color colors[256];
} Palette;

Palette* palReadFile(*char);
void palCloseFile(*Palette);


#endif
