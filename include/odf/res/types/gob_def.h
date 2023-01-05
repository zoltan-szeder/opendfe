#ifndef ODF_RES_TYPES_GOB_DEF_H
#define ODF_RES_TYPES_GOB_DEF_H

#include "odf/res/types/gob.h"
#include "odf/sys/types.h"

#define GOB_ARCHIVE_HEADER_FORMAT "%c4 %l4"

struct GobArchiveHeaders {
    char magic[4];
    uint32_t directoryOffset;
};

#define GOB_FILE_HEADER_FORMAT "%l4 %l4 %c13"

struct GobFileHeader {
    uint32_t offset;
    uint32_t length;
    char name[13];
};

#endif
