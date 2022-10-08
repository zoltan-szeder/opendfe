#ifndef ODF_RES_FILM_H
#define ODF_RES_FILM_H

#include "odf/res/types/film.h"

#include "odf/res/types/lfd.h"
#include "system/optional.h"

OptionalPtr* filmRead(LfdArchive* archive);
void filmPrint(const char* prefix, Film* film);

#endif
