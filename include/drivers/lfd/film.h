#ifndef DRIVERS_LFD_FILM_H
#define DRIVERS_LFD_FILM_H

#include "drivers/lfd.h"


typedef struct Film Film;

OptionalPtr* filmRead(LfdArchive* archive);
void filmPrint(const char* prefix, Film* film);

#endif
