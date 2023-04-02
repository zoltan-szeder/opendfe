#ifndef ODF_RES_TYPES_LRE_H
#define ODF_RES_TYPES_LRE_H

#include <stdint.h>

typedef enum RLEType {
    RLE0,
    RLE1
} RLEType;

typedef struct RLEConfig {
    RLEType type;
    uint32_t fileOffset;
    uint32_t offsetCount;
    uint32_t sequenceSize;
} RLEConfig;

#endif
