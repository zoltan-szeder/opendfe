#ifndef ODF_RES_TYPES_MSG_DEF_H
#define ODF_RES_TYPES_MSG_DEF_H

#include "odf/res/types/msg.h"
#include "odf/sys/types.h"

struct MsgFile {
    char magic[3];
    uint32_t majorVersion;
    uint32_t minorVersion;
    uint32_t numberOfMessages;
    Msg* messages;
};

struct Msg {
    uint32_t priority;
    char* value;
};

#endif
