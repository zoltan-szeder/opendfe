#ifndef ODF_RES_GOB_H
#define ODF_RES_GOB_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "odf/res/types/gob.h"

#include "odf/sys/types/optional.h"
#include "odf/sys/types/list.h"
#include "odf/sys/types/inmemoryfile.h"

OptionalPtr* gobOpenArchive(char* file_name);
int gobCloseArchive(GobArchive*);
int gobPrintArchive(GobArchive*);

size_t gobCountFiles(GobArchive*);
List* gobListFiles(GobArchive*);
GobFile* gobGetFile(GobArchive*, char*);
char* gobGetFileName(GobFile*);

InMemoryFile* gobReadFile(GobFile*);
void gobCloseFile(InMemoryFile*);

#endif
