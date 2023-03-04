#ifndef ODF_RES_FNT_H
#define ODF_RES_FNT_H

#include "odf/res/types/fnt.h"
#include "odf/sys/inmemoryfile.h"

OptionalOf(FNTFile*)* fntRead(InMemoryFile* file);

void fntClose(FNTFile* fntFile);

#endif
