#include <stdlib.h>
#include <string.h>

#include "drivers/rle.h"

void rleCopyBlock(uint8* dest, uint8 destOffset, uint8* src, uint8 srcOffset, uint8 length);
void rleSkipBlock(uint8* dest, uint8 destOffset, uint8 length, uint8 color);

uint8* rle0Decompress(uint8* src, int length, int width, int height) {
    uint8* dest = malloc(width*height*sizeof(uint8));

    int di = 0;
    int si = 0;
    while(si < length) {
        int blockSize = src[si];
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

uint8* rle1Decompress(uint8* src, int length, int width, int height) {
    uint8* dest = malloc(width*height*sizeof(uint8));

    int di = 0;
    int si = 0;
    while(si < length) {
        int blockSize = src[si];
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


void rleCopyBlock(uint8* dest, uint8 destOffset, uint8* src, uint8 srcOffset, uint8 length) {
    memcpy(dest + destOffset, src + srcOffset, length);
}

void rleSkipBlock(uint8* dest, uint8 destOffset, uint8 length, uint8 color) {
    for(int i = 0; i < length; i++) {
        dest[destOffset+i] = color;
    }
}
