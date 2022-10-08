#ifndef ODF_RES_FME_H
#define ODF_RES_FME_H

#include "odf/res/types/fme.h"

#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"


OptionalPtr* fmeOpenInMemoryFile(InMemoryFile*);
void fmeClose(FMEFile*);

#endif
