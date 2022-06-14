#include <stdlib.h>

#include "drivers/rle.h"

uint8* rle0Decompress(uint8* src, int length, int width, int height) {
    uint8* dest = malloc(width*height*sizeof(uint8));

    int di = 0;
    int si = 0;
    while(si < length) {
        int blockSize = src[si];
        si++;

        if(blockSize <= 128) {
            for(int i = 0; i < blockSize; i++) {
                dest[di] = src[si];
                di++;
                si++;
            }
        } else {
            for(int i = 0; i < blockSize - 128; i++) {
                dest[di] = '\00';
                di++;
            }
        }
    }

    return dest;
}
