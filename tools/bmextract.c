#include "odf/res/gob.h"
#include "odf/res/bm.h"
#include "odf/res/pal.h"

#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "odf/sys/file.h"


Palette* palExtract(char* gobFile, char* palFile) {
    GobArchive* palArchive = optionalGet(gobOpenArchive(gobFile));
    GobFile* palGobFile = optionalGet(gobGetFile(palArchive, palFile));
    InMemoryFile* palInMem = optionalGet(gobReadFile(palGobFile));

    Palette* pal = optionalGet(palOpen(palInMem));
    memoryTag(pal, "Palette");

    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    return pal;
}

BMFile* bmExtract(char* gobFile, char* bmFile) {
    GobArchive* bmArchive = optionalGet(gobOpenArchive(gobFile));
    GobFile* bmGobFile = optionalGet(gobGetFile(bmArchive, bmFile));
    InMemoryFile* bmInMem = optionalGet(gobReadFile(bmGobFile));

    BMFile* bm = optionalGet(bmOpenInMemoryFile(bmInMem));
    memoryTag(bm, "Bm");

    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    return bm;
}

int main(int argc, char** argv) {
    if(argc < 6) return 1;

    BMFile* bm = bmExtract(argv[1], argv[2]);
    Palette* pal = palExtract(argv[3], argv[4]);

    Image8Bit* image = bmCreateImage(bm, pal);
    uint32_t width = image->width;
    uint32_t height = image->height;
    uint32_t channels = image->channels;
    stbi_flip_vertically_on_write(true);
    stbi_write_png(argv[5], width, height, channels, image->data, width*channels);

    img8bDelete(image);
    bmClose(bm);
    palClose(pal);
}

