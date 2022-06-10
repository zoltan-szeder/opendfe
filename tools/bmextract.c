#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include "gl/init.h"

#include "display/main.h"
#include "display/model.h"
#include "glwindow.h"
#include "gl/init.h"
#include "file.h"
#include "gob.h"
#include "bm.h"
#include "pal.h"


Palette* palExtract(char* gobFile, char* palFile) {
    GobArchive* palArchive = gobOpenArchive(gobFile);
    InMemoryFile* palInMem = gobReadFile(gobGetFile(palArchive, palFile));

    Palette* pal = palOpenInMemoryFile(palInMem);

    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    return pal;
}

BMFile* bmExtract(char* gobFile, char* bmFile) {
    GobArchive* bmArchive = gobOpenArchive(gobFile);
    InMemoryFile* bmInMem = gobReadFile(gobGetFile(bmArchive, bmFile));

    BMFile* bm = bmOpenInMemoryFile(bmInMem);

    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    return bm;
}

int main(int argc, char** argv) {
    if(argc < 6) return 1;
    BMFile* bm = bmExtract(argv[1], argv[2]);
    if(bm == NULL) {
        printf("Could not find %s in %s\n", argv[2], argv[1]);
        return 1;
    }
    Palette* pal = palExtract(argv[3], argv[4]);
    if(pal == NULL) {
        printf("Could not find %s in %s\n", argv[4], argv[3]);
        return 1;
    }

    Image8Bit* image = bmCreateImage(bm, pal);
    uint32 width = image->width;
    uint32 height = image->height;
    uint32 channels = image->channels;
    stbi_write_png(argv[5], width, height, channels, image->data, width*channels);

    img8bDelete(image);
    bmClose(bm);
    palClose(pal);
}

