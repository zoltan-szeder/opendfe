#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "odf/ogl/main.h"
#include "odf/ogl/model.h"
#include "odf/ogl/texture.h"
#include "odf/sys/file.h"
#include "odf/res/gob.h"
#include "odf/res/bm.h"
#include "odf/res/pal.h"


int main(int argc, char** argv) {
    Display* display = dglCreateDisplay();

    if(argc < 5) return 1;
    OptionalPtr* optionalGobArchive = gobOpenArchive(argv[1]);
    if(optionalIsEmpty(optionalGobArchive)) {
        printf("%s\n", optionalGetMessage(optionalGobArchive));
        return 1;
    }
    GobArchive* bmArchive = optionalGet(optionalGobArchive);

    GobFile* bmFile = gobGetFile(bmArchive, argv[2]);
    InMemoryFile* bmInMem = gobReadFile(bmFile);
    BMFile* bm = bmOpenInMemoryFile(bmInMem);
    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    OptionalPtr* optionalPalArchive = gobOpenArchive(argv[3]);
    if(optionalIsEmpty(optionalPalArchive)) {
        printf("%s\n", optionalGetMessage(optionalPalArchive));
        return 1;
    }
    GobArchive* palArchive = optionalGet(optionalPalArchive);
    GobFile* paletteFile = gobGetFile(palArchive, argv[4]);
    InMemoryFile* palInMem = gobReadFile(paletteFile);
    Palette* pal = palOpenInMemoryFile(palInMem);


    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    Image8Bit* image = bmCreateImage(bm, pal);
    bmClose(bm);
    palClose(pal);

    int width;
    int height;
    int nrChannels;
    unsigned char *data = stbi_load("grenade.png", &width, &height, &nrChannels, 0);

    Image8Bit img;
    img.width = width;
    img.height = height;
    img.depth = 1;
    img.channels = nrChannels;
    img.data = data;

    DglTexture* texture = dglTextureCreate(&img);
    img8bDelete(image);


    float vertices[]= {
        // x      y     z     r     g     b     s     t
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GameState gamestate;
    gamestate.model = dglModelCreate(vertices, sizeof(vertices), indices, sizeof(indices));
    dglModelBindTexture(gamestate.model, texture);

    GameControl gameControl;
    gameControl.shouldExit = false;
    while(!gameControl.shouldExit)
    {
        dglReadInputs(display, &gameControl);

        dglDraw(display, &gamestate);
    }

    dglDestroyDisplay(display);
    dglModelDelete(gamestate.model);
    dglTextureDelete(texture);

    return 0;
}
