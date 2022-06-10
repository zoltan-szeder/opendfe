#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"
#include "inmemoryfile.h"
#include "bm.h"
#include "pal.h"
#include "math/vec.h"
#include <GL/glew.h>

bool bmIsTransparent(BMFile* bmFile, int colorIndex);

BMFile* bmOpenFile(char* file) {
    BMFile* bmFile = malloc(sizeof(BMFile));
    BMHeader* bmHeader = malloc(sizeof(BMHeader));

    bmFile->header = bmHeader;

    FILE* stream = fopen(file, "rb");

    fread(bmHeader, sizeof(BMHeader), 1, stream);

    bmFile->data = malloc(bmHeader->dataSize);
    fread(bmFile->data, bmHeader->dataSize, 1, stream);

    return bmFile;
}

BMFile* bmOpenInMemoryFile(InMemoryFile* file) {
    BMFile* bmFile = malloc(sizeof(BMFile));
    BMHeader* bmHeader = malloc(sizeof(BMHeader));

    memcpy(bmHeader, file->content, sizeof(BMHeader));
    bmFile->header = bmHeader;

    char* mbFileDataPtr = file->content + sizeof(BMHeader);

    bmFile->data = malloc(bmHeader->dataSize);

    memcpy(bmFile->data, mbFileDataPtr, bmHeader->dataSize);

    return bmFile;
}

void bmClose(BMFile* bmFile) {
    free(bmFile->header);
    free(bmFile->data);
    free(bmFile);
}

uint32 bmGlBindImageTexture(Image8Bit* img) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->height, img->width, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

int bmGetNormalizedPixelIndex(int w, int h, int i) {
    // BM contains pixels from bottomleft to topleft, then right
    int x = i/h;
    int y = h-1-(i%h);
    return x + w*y;
}

Image8Bit* bmCreateImage(BMFile* bmFile, Palette* palette) {
    int w = bmFile->header->sizeX;
    int h = bmFile->header->sizeY;
    Image8Bit* img = img8bCreate2D(w, h, 4);
    ucvec4* texture = (ucvec4*) img->data;

    for (int i =0; i < bmFile->header->dataSize; i++) {
        int pxi = bmGetNormalizedPixelIndex(w, h, i);

        unsigned char colorIndex = bmFile->data[i];
        ucvec3* paletteColor = palette->colors + colorIndex;
        ucvec4* pixel = texture + pxi;

        pixel->r = paletteColor->r;
        pixel->g = paletteColor->g;
        pixel->b = paletteColor->b;
        if(bmIsTransparent(bmFile, colorIndex)){
            pixel->a = 0;
        } else {
            pixel->a = 255;
        }
    }

    return img;
}

ucvec3* bmCreateRGBImage(BMFile* bmFile, Palette* palette) {
    int w = bmFile->header->sizeX;
    int h = bmFile->header->sizeY;
    ucvec3* texture = malloc(bmFile->header->dataSize * sizeof(ucvec3));

    for (int i =0; i<bmFile->header->dataSize; i++) {
        int pxi = bmGetNormalizedPixelIndex(w, h, i);

        unsigned char colorIndex = bmFile->data[i];
        ucvec3* paletteColor = palette->colors + colorIndex;
        ucvec3* pixel = texture + pxi;

        pixel->r = paletteColor->r;
        pixel->g = paletteColor->g;
        pixel->b = paletteColor->b;
    }

    return texture;
}
void bmDeleteTexture(ucvec4* texture) {
    free(texture);
}

bool bmIsTransparent(BMFile* bmFile, int colorIndex) {
    uint8 transparent = bmFile->header->transparent;
    return (transparent == BM_TRANSPARENT || transparent == BM_WEAPON)
        && colorIndex == '\x00';
}

void bmPrintFile(BMFile* bmFile) {
    BMHeader* bmHeader = bmFile->header;
    uint32* magic = (uint32*) &(bmHeader->magic);

    printf("magic: 0x%x\n", *magic);
    printf("sizeX: %d\n", bmHeader->sizeX);
    printf("sizeY: %d\n", bmHeader->sizeY);
    printf("idemX: %d\n", bmHeader->idemX);
    printf("idemY: %d\n", bmHeader->idemY);
    printf("transparent: 0x%x\n", bmHeader->transparent);
    printf("logSizeY: %d\n", bmHeader->logSizeY);
    printf("compressed: %d\n", bmHeader->compressed);
    printf("dataSize: %d\n", bmHeader->dataSize);
}
