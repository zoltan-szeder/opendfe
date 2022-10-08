#ifndef ODF_RES_FME_H
#define ODF_RES_FME_H

#include "odf/res/types/fme.h"

#include "system/optional.h"
#include "inmemoryfile.h"


OptionalPtr* fmeOpenInMemoryFile(InMemoryFile*);
void fmeClose(FMEFile*);

#endif
