#ifndef DRIVERS_LFD_H
#define DRIVERS_LFD_H
#include "system/optional.h"
#include "system/list.h"
#include "inmemoryfile.h"

typedef struct LfdArchive LfdArchive;
typedef struct LfdFile LfdFile;

typedef struct {
    char type[4];
    char name[8];
    uint32 length;
} LfdChunk;

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
