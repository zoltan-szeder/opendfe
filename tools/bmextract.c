#include <GL/glew.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "file.h"
#include "drivers/gob.h"
#include "drivers/bm.h"
#include "drivers/pal.h"


OptionalPtr* palExtract(char* gobFile, char* palFile) {
    OptionalPtr* optionalPalArchive = gobOpenArchive(gobFile);
    if(optionalIsEmpty(optionalPalArchive)) return optionalPalArchive;

    GobArchive* palArchive = optionalGet(optionalPalArchive);
    InMemoryFile* palInMem = gobReadFile(gobGetFile(palArchive, palFile));

    Palette* pal = palOpenInMemoryFile(palInMem);

    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    return optionalOf(pal);
}

OptionalPtr* bmExtract(char* gobFile, char* bmFile) {
    OptionalPtr* optionalBmArchive = gobOpenArchive(gobFile);
    if(optionalIsEmpty(optionalBmArchive)) return optionalBmArchive;

    GobArchive* bmArchive = optionalGet(optionalBmArchive);
    InMemoryFile* bmInMem = gobReadFile(gobGetFile(bmArchive, bmFile));

    BMFile* bm = bmOpenInMemoryFile(bmInMem);

    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    return optionalOf(bm);
}

int main(int argc, char** argv) {
    if(argc < 6) return 1;
    OptionalPtr* optionalBm = bmExtract(argv[1], argv[2]);
    if(optionalIsEmpty(optionalBm)) {
        printf("Could not find %s in %s\n", argv[2], argv[1]);
        printf("%s\n", optionalGetMessage(optionalBm));
        return 1;
    }
    OptionalPtr* optionalPal = palExtract(argv[3], argv[4]);
    if(optionalIsEmpty(optionalPal)) {
        printf("Could not find %s in %s\n", argv[4], argv[3]);
        printf("%s\n", optionalGetMessage(optionalPal));
        return 1;
    }

    BMFile* bm = optionalGet(optionalBm);
    Palette* pal = optionalGet(optionalPal);

    Image8Bit* image = bmCreateImage(bm, pal);
    uint32 width = image->width;
    uint32 height = image->height;
    uint32 channels = image->channels;
    stbi_flip_vertically_on_write(true);
    stbi_write_png(argv[5], width, height, channels, image->data, width*channels);

    img8bDelete(image);
    bmClose(bm);
    palClose(pal);
}

