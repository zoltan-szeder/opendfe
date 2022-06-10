#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "drivers/gob.h"
#include "types.h"

int gobReadArchive(GobArchive*, FILE*);
GobFile* gobReadArchiveFile(GobArchive*, GobFile*);
GobFile* gobReadArchiveFiles(GobArchive*);
uint32 gobReadArchiveFileCount(GobArchive*);

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

    GobArchive* archive = malloc(sizeof(GobArchive));
    if(NULL == archive) return NULL;

    gobReadArchive(archive, gob_file);

    return archive;
}

int gobCloseArchive(GobArchive* archive) {
    FILE* stream = archive->stream;

    free(archive->files);
    free(archive);
    
    return fclose(stream);
}

uint32 gobCountFiles(GobArchive* archive){
    return archive->fileCount;
}

GobFile* gobListFiles(GobArchive* archive){
    return archive->files;
}

GobFile* gobGetFile(GobArchive* archive, char* file_name){
    for(int i = 0; i < archive->fileCount; i++) {
        GobFile* gob_file = &(archive->files[i]);
        if(strcmp(file_name, gob_file->name) == 0) return gob_file;
    }

    errno = ENOENT;
    return NULL;
}

char* gobGetFileName(GobFile* gob_file) {
    return gob_file->name;
}

InMemoryFile* gobReadFile(GobFile* gob_file){
    GobArchive* archive = gob_file->parent;
    FILE* stream = archive->stream;

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


int gobPrintArchive(GobArchive* archive){
    uint32* magic = (uint32*) &(archive->magic);

    printf("Magic: 0x%x\n", *magic);
    printf("Directory offset: %d\n", archive->directoryOffset);
    printf("Number of Files: %d\n", archive->fileCount);
    printf("Files:\n");
    for(int i = 0; i < archive->fileCount; i++) {
        GobFile* gob_file = &(archive->files[i]);
        printf("\t%s (%d-%d)\n", gob_file->name, gob_file->offset, gob_file->offset + gob_file->length);
    }

    return 0;
}

int gobReadArchive(GobArchive* archive, FILE* stream) {
    archive->stream = stream;

    int objects = 0;
    objects = fread(archive, sizeof(uint32), 2, stream);
    if(!objects) return 0;

    archive->fileCount = gobReadArchiveFileCount(archive);
    if(archive->fileCount < 0) return 0;

    archive->files = gobReadArchiveFiles(archive);
    if(!archive->files) return 0;

    return 1;
}

uint32 gobReadArchiveFileCount(GobArchive* archive) {
    FILE* stream = archive->stream;

    int failure = fseek(stream, (long) archive->directoryOffset, SEEK_SET);
    if(failure) return -1;

    uint32 count;
    int objects = fread_uint32(&(count), stream);
    if(!objects) return -1;

    return count;
}

GobFile* gobReadArchiveFiles(GobArchive* archive) {
    uint32 count = archive->fileCount;

    GobFile* files = malloc(count*sizeof(GobFile));
    if(!files) return NULL;

    for(int i = 0; i < count; i++) {
        GobFile* file = &(files[i]);

        gobReadArchiveFile(archive, file);
        if(file == NULL) {
            free(files);
            return NULL;
        }
    }

    return files;
}


GobFile* gobReadArchiveFile(GobArchive* archive, GobFile* gob_file) {
    FILE* stream = archive->stream;

    gob_file->parent = archive;
    int objects = fread(gob_file, 2*sizeof(uint32) + 13*sizeof(char), 1, stream);
    if(!objects) return NULL;

    return gob_file;
}

int fread_uint32(uint32* number, FILE* stream) {
    return fread(number, 4, 1, stream);
}
