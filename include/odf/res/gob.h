#ifndef ODF_RES_GOB_H
#define ODF_RES_GOB_H

#include <stdio.h>
#include <stddef.h>
#include "odf/res/types/gob.h"

#include "types.h"
#include "system/optional.h"
#include "system/list.h"
#include "inmemoryfile.h"

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
