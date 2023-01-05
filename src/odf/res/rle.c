#include <stdlib.h>
#include <string.h>

#include "odf/res/rle.h"
#include "odf/sys/memory.h"

void rleCopyBlock(uint8_t* dest, uint8_t destOffset, uint8_t* src, uint8_t srcOffset, uint8_t length);
void rleSkipBlock(uint8_t* dest, uint8_t destOffset, uint8_t length, uint8_t color);

uint8_t* rle0Decompress(uint8_t* src, int32_t length, int32_t width, int32_t height) {
    uint8_t* dest = memoryAllocateWithTag(width*height*sizeof(uint8_t), "odf/res/ldf/rle0Decompress");

    int32_t di = 0;
    int32_t si = 0;
    while(si < length) {
        int32_t blockSize = src[si];
        si++;

        if(blockSize <= 128) {
            rleCopyBlock(dest, di, src, si, blockSize);
            si += blockSize;
            di += blockSize;
        } else {
            blockSize -= 128;
            char color = '\x00';

            rleSkipBlock(dest, di, blockSize, color);
            di += blockSize;
        }
    }

    return dest;
}

uint8_t* rle1Decompress(uint8_t* src, int32_t length, int32_t width, int32_t height) {
    uint8_t* dest = memoryAllocateWithTag(width*height*sizeof(uint8_t), "odf/res/ldf/rle0Decompress");

    int32_t di = 0;
    int32_t si = 0;
    while(si < length) {
        int32_t blockSize = src[si];
        si++;

        if(blockSize <= 128) {
            rleCopyBlock(dest, di, src, si, blockSize);
            si += blockSize;
            di += blockSize;
        } else {
            blockSize -= 128;
            char color = src[si];
            si++;

            rleSkipBlock(dest, di, blockSize, color);
            di += blockSize;
        }
    }

    return dest;
}


void rleCopyBlock(uint8_t* dest, uint8_t destOffset, uint8_t* src, uint8_t srcOffset, uint8_t length) {
    memcpy(dest + destOffset, src + srcOffset, length);
}

void rleSkipBlock(uint8_t* dest, uint8_t destOffset, uint8_t length, uint8_t color) {
    for(int i = 0; i < length; i++) {
        dest[destOffset+i] = color;
    }
}
