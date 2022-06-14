#include <stdio.h>
#include <stdlib.h>

#include "dgl/main.h"
#include "dgl/model.h"
#include "dgl/texture.h"
#include "drivers/pal.h"
#include "drivers/bm.h"
#include "drivers/gob.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
    Display* display = dglCreateDisplay();

    if(argc < 5) return 1;
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

    DglTexture* texture = dglTextureCreate(image);
    img8bDelete(image);

    float vertices[]= {
    //     x      y     z       r     g     b       s     t
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
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
