#ifndef ODF_RES_MSG_H
#define ODF_RES_MSG_H

#include "odf/res/types/msg.h"

#include <stdio.h>

#include "odf/sys/types/optional.h"
#include "odf/sys/types/inmemoryfile.h"


OptionalPtr* msgOpenFile(InMemoryFile* file);

/*
 * Returns: Optional of Msg
 */
OptionalPtr* msgGet(MsgFile* file, size_t index);
void msgCloseFile(MsgFile*);

void msgPrint(FILE* stream, MsgFile* msgFile);

#endif
