#ifndef ODF_RES_GOB_H
#define ODF_RES_GOB_H

#include <stdio.h>
#include <stddef.h>
#include "odf/res/types/gob.h"

#include "odf/sys/types.h"
#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/inmemoryfile.h"

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
