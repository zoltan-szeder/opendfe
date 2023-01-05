#ifndef ODF_RES_MSG_H
#define ODF_RES_MSG_H

#include "odf/res/types/msg.h"
#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"


OptionalPtr* msgOpenFile(InMemoryFile* file);
void msgCloseFile(MsgFile*);

#endif
