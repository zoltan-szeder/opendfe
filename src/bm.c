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

uint32 bmGlBindImageTexture(BMFile* bmFile, Palette* palette) {
    ucvec4* data = bmCreateTexture(bmFile, palette);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmFile->header->sizeY, bmFile->header->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    bmDeleteTexture(data);
    return texture;
}

ucvec4* bmCreateTexture(BMFile* bmFile, Palette* palette) {
    ucvec4* texture = malloc(bmFile->header->dataSize * sizeof(ucvec4));

    for (int i =0; i<bmFile->header->dataSize; i++) {
        int colorIndex = bmFile->data[i];
        ucvec3* paletteColor = palette->colors + bmFile->data[i];
        ucvec4* pixel = texture + i;

        pixel->r = paletteColor->r;
        pixel->g = paletteColor->g;
        pixel->b = paletteColor->b;
        if(bmIsTransparent(bmFile, colorIndex))
            pixel->a = 0;
        else
            pixel->a = 1;
    }

    return texture;
}

void bmDeleteTexture(ucvec4* texture) {
    free(texture);
}

bool bmIsTransparent(BMFile* bmFile, int colorIndex) {
    uint8 transparent = bmFile->header->transparent;
    return (transparent == BM_TRANSPARENT && colorIndex == '\x00')
        || (transparent == BM_WEAPON      && colorIndex == '\x08');
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
