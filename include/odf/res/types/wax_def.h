#ifndef ODF_RES_TYPES_WAX_DEF_H
#define ODF_RES_TYPES_WAX_DEF_H

#include "odf/res/types/wax.h"
#include <stdint.h>

#define UINT32_X_32 "%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4" \
    "%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4%l4"

struct WAXFile {
    WAXHeader* header;
};

#define WAX_HEADER_FORMAT "%l4%l4%l4%l4%l4%l4%l4%c4" UINT32_X_32
struct WAXHeader {
    uint32_t version;
    uint32_t seqs;
    uint32_t frames;
    uint32_t cells;
    uint32_t scaleX;
    uint32_t scaleY;
    uint32_t extraLight;
    uint32_t pad;
    uint32_t* waxes[32];
};

#define WAX_FORMAT "%l4%l4%l4%l4%c12" UINT32_X_32
struct WAX {
    uint32_t width;
    uint32_t height;
    uint32_t framerate;
    uint32_t frames;
    uint32_t pads[3];
    uint32_t* sequences[32];
};

#define WAX_SEQUENCE_FORMAT "%l4%l4%l4%l4" UINT32_X_32
struct WAXSequence {
    uint32_t pad[4];
    uint32_t frames[32];
};

#define WAX_FRAME_FORMAT "%l4%l4%l4%l4%l4%l4%c8"
struct WAXFrame {
    uint32_t insertX;
    uint32_t insertY;
    uint32_t flip;
    uint32_t cell;
    uint32_t width;
    uint32_t height;
    uint32_t pad[2];
};

#define WAX_CELL_FORMAT "%l4%l4%l4%l4%l4%c4"
struct WAXCell {
    uint32_t width;
    uint32_t height;
    uint32_t compressed;
    uint32_t dataSize;
    uint32_t offset;
    uint32_t pad;
};

#endif
