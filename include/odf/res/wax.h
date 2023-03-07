#ifndef ODF_RES_WAX_H
#define ODF_RES_WAX_H

#include "odf/res/types/wax.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

OptionalOf(WAXFile*)* waxReadInMemoryFile(InMemoryFile* file);
void waxClose(WAXFile* wax);

#endif
