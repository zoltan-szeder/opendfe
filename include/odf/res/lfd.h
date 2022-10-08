#ifndef ODF_RES_LFD_H
#define ODF_RES_LFD_H

#include "odf/res/types/lfd.h"
#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/inmemoryfile.h"

OptionalPtr* lfdOpenArchive(char* file_name);
void lfdCloseArchive(LfdArchive*);
void lfdPrintArchive(LfdArchive*);
void lfdPrintChunk(const char* prefix, LfdChunk* chunk);

uint32 lfdCountFiles(LfdArchive*);
List* lfdListFiles(LfdArchive*);
LfdFile* lfdGetFile(LfdArchive*, char*);
char* lfdGetFileName(LfdFile*);

OptionalPtr* lfdReadFile(LfdFile*);
void lfdCloseFile(InMemoryFile*);

#endif
