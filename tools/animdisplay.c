#include <stdio.h>
#include <stdlib.h>

#include "odf/res/lfd.h"
#include "odf/res/film.h"


int main(int argc, char** argv) {
    LfdArchive* archive = optionalGet(lfdOpenArchive(argv[1]));
    Film* film = optionalGet(filmRead(archive));

    filmPrint("", film);

    lfdCloseArchive(archive);
    return 0;
}
