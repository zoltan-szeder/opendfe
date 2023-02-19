#ifndef ODF_RES_BM_H
#define ODF_RES_BM_H

#include "odf/res/types/bm.h"
#include "odf/res/types/pal.h"
#include "odf/sys/types/optional.h"

#include "odf/sys/inmemoryfile.h"
#include "odf/sys/img.h"
#include "odf/sys/types/list.h"

BMFile* bmOpenFile(char*);
OptionalOf(BMFile*)* bmOpenInMemoryFile(InMemoryFile*);
void bmClose(BMFile*);

void bmPrintFile(BMFile*);

Image8Bit* bmCreateImage(BMFile*, Palette*);
ListOf(Image8Bit*)* bmCreateImages(BMFile* bmFile, Palette* palette);


#endif
