#ifndef ODF_RES_BM_H
#define ODF_RES_BM_H

#include "odf/res/types/bm.h"
#include "odf/res/types/pal.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/img.h"

BMFile* bmOpenFile(char*);
BMFile* bmOpenInMemoryFile(InMemoryFile*);
void bmClose(BMFile*);

void bmPrintFile(BMFile*);

Image8Bit* bmCreateImage(BMFile*, Palette*);

#endif
