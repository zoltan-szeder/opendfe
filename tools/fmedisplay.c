#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "odf/res/pal.h"
#include "odf/res/fme.h"
#include "odf/res/gob.h"

#include "odf/sys/gc.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/log.h"

#include "odf/ogl/main.h"
#include "odf/ogl/model.h"
#include "odf/ogl/texture.h"

#include "odf/ctrl/ingameevent.h"
#include "odf/ctrl/glfw/gamepad.h"

Palette* palExtract(char* gobFile, char* palFile) {
    GobArchive* palArchive = optionalGet(gobOpenArchive(gobFile));
    GobFile* palGobFile = optionalGet(gobGetFile(palArchive, palFile));
    InMemoryFile* palInMem = optionalGet(gobReadFile(palGobFile));

    memoryTag(palInMem, "PaletteInMemory");

    OptionalPtr* optionalPal = palOpen(palInMem);
    Palette* pal = optionalGet(optionalPal);
    memoryTag(pal, "Palette");

    gobCloseFile(palInMem);
    gobCloseArchive(palArchive);

    return pal;
}

FMEFile* fmeExtract(char* gobFile, char* fmeFile) {
    GobArchive* archive = optionalGet(gobOpenArchive(gobFile));
    GobFile* file = optionalGet(gobGetFile(archive, fmeFile));
    InMemoryFile* fmeInMemFile = optionalGet(gobReadFile(file));
    FMEFile* fme = optionalGet(fmeOpenInMemoryFile(fmeInMemFile));

    gobCloseFile(fmeInMemFile);
    gobCloseArchive(archive);

    fmePrint(fme);

    return fme;
}

int main(int argc, char** argv) {

    if(argc < 5) return 1;
    logSetLevel(TRACE);
    FMEFile* fme = fmeExtract(argv[1], argv[2]);
    Palette* pal = palExtract(argv[3], argv[4]);

    Image8Bit* image = fmeExtractImage(fme, pal);
    fmeClose(fme);
    palClose(pal);

    Display* display = dglCreateDisplay();

    DglTexture* texture = dglTextureCreate(image);

    float sW = 640;
    float sH = 480;

    float x1 = -5.0 * image->width / sW;
    float x2 = 5.0 * image->width / sW;
    float y1 = -5.0 * image->height / sH;
    float y2 = 5.0 * image->height / sH;

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

    GameControl gameControl;
    gameControl.shouldExit = false;

    ctrlGlfwGamepadInit();
    while(!gameControl.shouldExit)
    {
        dglReadInputs(display, &gameControl);

        dglModelBindTexture(gamestate.model, texture);

        dglDraw(display, &gamestate);
    }

    dglDestroyDisplay(display);
    dglModelDelete(gamestate.model);
    dglTextureDelete(texture);

    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }

    return 0;
}
