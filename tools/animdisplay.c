#include <stdio.h>
#include <stdlib.h>

#include "odf/res/lfd.h"
#include "drivers/lfd/film.h"


int main(int argc, char** argv) {
    OptionalPtr* optArchive = lfdOpenArchive(argv[1]);
    if(optionalIsEmpty(optArchive)) {
        optionalPrint(stderr, (void*) optArchive);
        return 1;
    }
    LfdArchive* archive = optionalGet(optArchive);

    OptionalPtr* optFilm =  filmRead(archive);
    if(optionalIsEmpty(optFilm)) {
        optionalPrint(stderr, (void*) optFilm);
        return 1;
    };
    Film* film = optionalGet(optFilm);

    filmPrint("", film);

    lfdCloseArchive(archive);
    return 0;
}
