#ifndef ODF_RES_TYPES_MSG_DEF_H
#define ODF_RES_TYPES_MSG_DEF_H

#include "odf/res/types/msg.h"
#include "odf/sys/types/list.h"
#include "odf/sys/types.h"

struct MsgFile {
    char magic[3];
    uint32_t majorVersion;
    uint32_t minorVersion;
    List* messages;
};

struct Msg {
    size_t index;
    size_t priority;
    char* value;
};

#endif
