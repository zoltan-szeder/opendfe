
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "odf/res/wax.h"
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

WAXFile* waxExtract(char* gobFile, char* waxFile) {
    GobArchive* archive = optionalGet(gobOpenArchive(gobFile));
    GobFile* file = optionalGet(gobGetFile(archive, waxFile));
    InMemoryFile* inMemFile = optionalGet(gobReadFile(file));
    WAXFile* wax = optionalGet(waxReadInMemoryFile(inMemFile));
    memoryTag(wax, "WAX");

    gobCloseFile(inMemFile);
    gobCloseArchive(archive);

    return wax;
}

int main(int argc, char** argv) {

    if(argc < 3) return 1;
    logSetLevel(DEBUG);
    WAXFile* wax = waxExtract(argv[1], argv[2]);

    waxClose(wax);


    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }

    return 0;
}
