#ifndef GOB_H
#define GOB_H

#include <stdio.h>
#include <stddef.h>
#include "types.h"

typedef struct GobArchive GobArchive;
typedef struct GobFile GobFile;
typedef struct {
    uint32 length;
    char* content;
} InMemoryFile;

GobArchive* gobOpenArchive(char* file_name);
int gobCloseArchive(GobArchive*);
int gobPrintArchive(GobArchive*);

uint32 gobCountFiles(GobArchive*);
GobFile* gobListFiles(GobArchive*);
GobFile* gobGetFile(GobArchive*, char*);
char* gobGetFileName(GobFile*);

InMemoryFile* gobReadFile(GobFile*);
void gobCloseFile(InMemoryFile*);

#endif
