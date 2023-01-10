#include <string.h>
#include "odf/sys/memory.h"
#include "odf/res/film.h"
#include "odf/res/lfd.h"
#include "odf/sys/inmemoryfile.h"
#include "odf/sys/list.h"

struct Film {
    LfdChunk* header;
};

OptionalPtr* filmFindFirst(LfdArchive* archive);
OptionalPtr* filmReadFilmFile(LfdArchive* archive);

OptionalPtr* filmRead(LfdArchive* archive) {
    OptionalPtr* optInMemFilm =  filmReadFilmFile(archive);
    if(optionalIsEmpty(optInMemFilm)) return optInMemFilm;
    InMemoryFile* inMemFilm = optionalGet(optInMemFilm);

    OptionalPtr* optHeader =  inMemFileReadStruct(inMemFilm, "%c4 %c8 %l4");
    if(optionalIsEmpty(optHeader)) return optHeader;

    Film* film = memoryAllocateWithTag(sizeof(Film), "odf/res/film/Film");
    film->header = optionalGet(optHeader);

    return optionalOf(film);
}


OptionalPtr* filmReadFilmFile(LfdArchive* archive) {
    OptionalPtr* optFilmFile = filmFindFirst(archive);
    if(optionalIsEmpty(optFilmFile)) return optFilmFile;
    LfdFile* filmFile = optionalGet(optFilmFile);

    return lfdReadFile(filmFile);
}

OptionalPtr* filmFindFirst(LfdArchive* archive) {
    List* files = lfdListFiles(archive);
    for(int i = 0; i < lfdCountFiles(archive); i++) {
        OptionalPtr* optFile =  listGet(files, i);
        if(optionalIsEmpty(optFile)) return optFile;

        LfdFile* file = optionalGet(optFile);
        if(strncmp("FILM", lfdGetFileName(file), 4)) return optionalOf(file);
    }

    return optionalEmpty("filmFindFirst - Could not find FILM element in LFD archive");
}


void filmPrint(const char* prefix, Film* film){
    lfdPrintChunk(prefix, film->header);
}
