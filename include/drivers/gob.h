#ifndef DRIVERS_GOB_H
#define DRIVERS_GOB_H

#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "system/optional.h"
#include "system/list.h"
#include "inmemoryfile.h"

typedef struct GobArchive GobArchive;
typedef struct GobFile GobFile;

OptionalPtr* gobOpenArchive(char* file_name);
int gobCloseArchive(GobArchive*);
int gobPrintArchive(GobArchive*);

uint32 gobCountFiles(GobArchive*);
List* gobListFiles(GobArchive*);
GobFile* gobGetFile(GobArchive*, char*);
char* gobGetFileName(GobFile*);

InMemoryFile* gobReadFile(GobFile*);
void gobCloseFile(InMemoryFile*);

#endif
