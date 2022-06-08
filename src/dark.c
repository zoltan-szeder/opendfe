#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl/init.h"

#include "display/main.h"
#include "display/model.h"
#include "glwindow.h"
#include "gl/init.h"
#include "file.h"
#include "gob.h"
#include "bm.h"
#include "pal.h"


int main(int argc, char** argv) {
    Display* display = dglCreateDisplay();

    if(argc < 5) return 1;
    GobArchive* bmArchive = gobOpenArchive(argv[1]);
    GobFile* bmFile = gobGetFile(bmArchive, argv[2]);
    InMemoryFile* bmInMem = gobReadFile(bmFile);
    BMFile* bm = bmOpenInMemoryFile(bmInMem);
    gobCloseFile(bmInMem);
    gobCloseArchive(bmArchive);

    GobArchive* palArchive = gobOpenArchive(argv[3]);
    GobFile* paletteFile = gobGetFile(palArchive, argv[4]);
    InMemoryFile* palInMem = gobReadFile(paletteFile);
    Palette* pal = palOpenInMemoryFile(palInMem);


    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    uint32 texture = bmGlBindImageTexture(bm, pal);
    bmClose(bm);
    palClose(pal);

    GameState gamestate;
    GameControl gameControl;
    gameControl.shouldExit = false;

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

    gamestate.texture = texture;
    gamestate.model = modelCreate(vertices, sizeof(vertices), indices, sizeof(indices));

    while(!gameControl.shouldExit)
    {
        *dglReadInputs(display, &gameControl);

        dglDraw(display, &gamestate);
    }

    dglDestroyDisplay(display);

    return 0;
}

