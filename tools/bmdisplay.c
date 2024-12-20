#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "odf/res/pal.h"
#include "odf/res/bm.h"
#include "odf/res/gob.h"

#include "odf/sys/gc.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/log.h"

#include "odf/ogl/main.h"
#include "odf/ogl/model.h"
#include "odf/ogl/texture.h"
#include "odf/ogl/dyntex.h"

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

    if(argc < 5) return 1;
    logSetLevel(TRACE);
    BMFile* bm = bmExtract(argv[1], argv[2]);
    Palette* pal = palExtract(argv[3], argv[4]);

    ListOf(Image8Bit*)* images = bmCreateImages(bm, pal);
    bmClose(bm);
    palClose(pal);

    Display* display = dglCreateDisplay();


    OGLDynamicTexture* dyntex = oglDtCreateTexture();
    forEachListItem(Image8Bit*, image, images, {
        oglDtAddTexture(dyntex, dglTextureCreate(image));
    });

    Image8Bit* image = optionalGet(listGet(images, 0));

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

    forEachListItem(Image8Bit*, imageToDelete, images, {
        img8bDelete(imageToDelete);
    });


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
        ctrlGlfwGamepadProcessAllIngameEvents();

        ListOf(CtrlEvent*)* events = ctrlInGameEventFetchAll();
        forEachListItem(CtrlEvent*, event, events, {
            if(event->type == CTRL_MOV_ACTIVATE && event->action == CTRL_PRESS) {
                oglDtSwitchTexture(dyntex);
            }
        });
        ctrlInGameEventReleaseAll(events);


        DglTexture* texture = oglDtGetCurrentTexture(dyntex);
        dglModelBindTexture(gamestate.model, texture);

        dglDraw(display, &gamestate);
    }

    dglDestroyDisplay(display);
    dglModelDelete(gamestate.model);

    oglDtDeleteTextures(dyntex);

    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }

    return 0;
}
