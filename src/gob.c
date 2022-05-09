#include "../headers/gob.h"
#include "../headers/types.h"
#include <stdlib.h>
#include <string.h>

int gobReadArchive(GobArchive*, FILE*);
int gobReadArchiveFile(GobFile*, FILE*);

int fread_uint32(uint32* number, FILE* stream);

struct GobArchive {
    char magic[4];
    uint32 directoryOffset;
    uint32 fileCount;

    GobFile* files;

    FILE* stream;
};

struct GobFile {
    uint32 offset;
    uint32 length;
    char name[13];

    GobArchive* parent;
};

GobArchive* gobOpenArchive(char* filename) {
    FILE* gob_file = fopen(filename, "rb");
    if(NULL == gob_file) return NULL;

    GobArchive* gob_archive = malloc(sizeof(GobArchive));
    if(NULL == gob_archive) return NULL;

    gobReadArchive(gob_archive, gob_file);

    return gob_archive;
}

int gobCloseArchive(GobArchive* gob_archive) {
    FILE* stream = gob_archive->stream;

    free(gob_archive->files);
    free(gob_archive);
    
    return fclose(stream);
}

uint32 gobCountFiles(GobArchive* gob_archive){
    return gob_archive->fileCount;
}

GobFile* gobListFiles(GobArchive* gob_archive){
    return gob_archive->files;
}

GobFile* gobGetFile(GobArchive* gob_archive, char* file_name){
    for(int i = 0; i < gob_archive->fileCount; i++) {
        GobFile* gob_file = &(gob_archive->files[i]);
        if(strcmp(file_name, gob_file->name) == 0) return gob_file;
    }
    return NULL;
}

InMemoryFile* gobReadFile(GobFile* gob_file){
    GobArchive* gob_archive = gob_file->parent;
    FILE* stream = gob_archive->stream;

    int failure = fseek(stream, gob_file->offset, SEEK_SET);
    if(failure) return NULL;

    char* content = malloc(gob_file->length);
    if(!content) return NULL;

    int objects = fread(content, gob_file->length, 1, stream);
    if(objects == 0) return NULL;
    content[gob_file->length-1] = (char) 0;

    InMemoryFile* file = malloc(sizeof(InMemoryFile));
    if(!file) return NULL;

    file->content = content;
    file->length = gob_file->length;

    return file;
}

void gobCloseFile(InMemoryFile* file) {
    free(file->content);
    free(file);
}


int gobPrintArchive(GobArchive* gob_archive){
    uint32* magic = (uint32*) &(gob_archive->magic);

    printf("Magic: 0x%x\n", *magic);
    printf("Directory offset: %d\n", gob_archive->directoryOffset);
    printf("Number of Files: %d\n", gob_archive->fileCount);
    printf("Files:\n");
    for(int i = 0; i < gob_archive->fileCount; i++) {
        GobFile* gob_file = &(gob_archive->files[i]);
        printf("\t%s (%d-%d)\n", gob_file->name, gob_file->offset, gob_file->offset + gob_file->length);
    }

    return 0;
}

int gobReadArchive(GobArchive* gob_archive, FILE* stream) {
    gob_archive->stream = stream;

    int objects = 0;
    objects = fread(gob_archive, sizeof(uint32), 2, stream);
    if(!objects) return 0;

    int failure = fseek(stream, (long) gob_archive->directoryOffset, SEEK_SET);
    if(failure) return 0;
    objects = fread_uint32(&(gob_archive->fileCount), stream);
    if(!objects) return 0;

    gob_archive->files = malloc(gob_archive->fileCount*sizeof(GobFile));
    if(!gob_archive->files) return 0;
    for(int i = 0; i < gob_archive->fileCount; i++) {
        GobFile* gob_file = &(gob_archive->files[i]);
        gob_file->parent = gob_archive;
        objects = gobReadArchiveFile(gob_file, stream);
        if(!objects) return 0;
    }

    return 1;
}

int gobReadArchiveFile(GobFile* gob_file, FILE* stream) {
    int objects = fread(gob_file, 2*sizeof(uint32) + 13*sizeof(char), 1, stream);
    if(!objects) return 0;

    return 1;
}

int fread_uint32(uint32* number, FILE* stream) {
    return fread(number, 4, 1, stream);
}
