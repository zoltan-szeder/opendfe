#ifndef BM_H
#define BM_H
#include "pal.h"
#include "inmemoryfile.h"

typedef struct BMFile BMFile;

BMFile* bmOpenFile(char*);
BMFile* bmOpenInMemoryFile(InMemoryFile*);
void bmCloseFile(BMFile*);

uint32 bmGlBindImageTexture(BMFile*, Palette*);
void bmPrintFile(BMFile*);

#endif
