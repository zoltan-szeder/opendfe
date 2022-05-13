#ifndef BM_H
#define BM_H
#include "pal.h"

typedef struct BMFile BMFile;

BMFile* bmOpenFile(char*);
void bmCloseFile(BMFile*);

uint32 bmGlBindImageTexture(BMFile*, Palette*);
void bmPrintFile(BMFile*);

#endif
