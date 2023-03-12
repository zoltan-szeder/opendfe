#ifndef ODF_RES_TYPES_WAX_DEF_H
#define ODF_RES_TYPES_WAX_DEF_H

#include "odf/res/types/wax.h"
#include <stdint.h>
#include "odf/sys/list.h"

#define UINT32_X_32 "%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4" \
    "%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4"

struct WAXFile {
    WAXHeader* header;
    ListOf(WAX*)* waxList;
};

#define WAX_HEADER_FORMAT "%l4%l4%l4%l4%l4%l4%l4%c4"
struct WAXHeader {
    uint32_t version;
    uint32_t seqs;
    uint32_t frames;
    uint32_t cells;
    uint32_t scaleX;
    uint32_t scaleY;
    uint32_t extraLight;
    uint32_t pad;
};

#define WAX_FORMAT "%l4%l4%l4%l4%c12"
struct WAX {
    uint32_t width;
    uint32_t height;
    uint32_t framerate;
    uint32_t frames;
    uint32_t pads[3];
    ListOf(WAXSequence*)* sequences;
};

#define WAX_SEQUENCE_FORMAT "%l4%l4%l4%l4"
struct WAXSequence {
    uint32_t pad[4];
    ListOf(WAXFrame*)* frames;
};

#define WAX_FRAME_FORMAT "%l4%l4%l4%l4%l4%l4%c8"
struct WAXFrame {
    uint32_t insertX;
    uint32_t insertY;
    uint32_t flip;
    uint32_t cellOffset;
    uint32_t width;
    uint32_t height;
    uint32_t pad[2];
    WAXCell* cell;
};

#define WAX_CELL_FORMAT "%l4%l4%l4%l4%l4%c4"
struct WAXCell {
    uint32_t width;
    uint32_t height;
    uint32_t compressed;
    uint32_t dataSize;
    uint32_t offset;
    uint32_t pad;
    uint8_t* data;
};

#endif
