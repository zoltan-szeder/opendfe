#ifndef ODF_RES_3DO_H
#define ODF_RES_3DO_H


#include "odf/res/types/3do.h"

#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

OptionalOf(3DOFile)* tdoOpenFile(InMemoryFile* sb);
void tdoCloseFile(TDOFile* tdoFile);






#endif // ODF_RES_3DO_H
