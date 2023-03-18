#ifndef ODF_RES_FME_H
#define ODF_RES_FME_H

#include "odf/res/types/fme.h"

#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"
#include "odf/res/types/pal.h"
#include "odf/sys/img.h"


OptionalPtr* fmeOpenInMemoryFile(InMemoryFile*);
Image8Bit* fmeExtractImage(FMEFile*, Palette*);
void fmePrint(FMEFile*);
void fmeClose(FMEFile*);

#endif
