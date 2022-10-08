#include <stdio.h>
#include <stdlib.h>

#include "dgl/main.h"
#include "dgl/model.h"
#include "dgl/texture.h"
#include "odf/res/pal.h"
#include "odf/res/bm.h"
#include "odf/res/gob.h"

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
    OptionalPtr* optionalGobArchive = gobOpenArchive(gobFile);
    if(optionalIsEmpty(optionalGobArchive)) return optionalGobArchive;

    GobArchive* bmArchive = optionalGet(optionalGobArchive);
    InMemoryFile* bmInMem = gobReadFile(gobGetFile(bmArchive, bmFile));

    BMFile* bm = bmOpenInMemoryFile(bmInMem);
    bmPrintFile(bm);

    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    return optionalOf(bm);
}

int main(int argc, char** argv) {
    Display* display = dglCreateDisplay();

    if(argc < 5) return 1;
    OptionalPtr* optionalBm = bmExtract(argv[1], argv[2]);
    if(optionalIsEmpty(optionalBm)) {
        printf("Could not find %s in %s\n", argv[2], argv[1]);
        printf("%s\n", optionalGetMessage(optionalBm));
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

    DglTexture* texture = dglTextureCreate(image);

    float sW = 640;
    float sH = 480;

    float sX = 2.0/sW;
    float sY = -2.0/sH;

    float tX = -1;
    float tY = 1;

    float iW = sX*image->width *2;
    float iH = sY*image->height*2.41;

    float iX = sX*330;
    float iY = sY*264;

    float pX = sX*413;
    float pY = sY*375;

    float x1 = tX + pX - iW/2.0;
    float x2 = tX + pX + iW/2.0;
    float y1 = tY + pY + iH/2.0;
    float y2 = tY + pY - iH/2.0;

    img8bDelete(image);


    printf("%g, %g, %g, %g\n", x1, x2, y1, y2);

    float vertices[]= {
    //     x      y     z       r     g     b       s     t
           x2,    y2, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
           x2,    y1, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
           x1,    y1, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
           x1,    y2, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
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
