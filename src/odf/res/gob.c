#include "odf/res/gob.h"
#include "odf/res/types/gob_def.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "odf/sys/types.h"
#include "odf/sys/file.h"
#include "odf/sys/memory.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"
#include "odf/sys/inmemoryfile.h"

OptionalPtr* gobReadArchive(FILE*);
OptionalPtr* gobReadArchiveFile(GobArchive*);
OptionalPtr* gobReadArchiveFiles(GobArchive*, uint32_t fileCount);
uint32_t gobReadArchiveFileCount(GobArchive* archive);

struct GobArchive {
    FILE* stream;
    GobArchiveHeaders* headers;
    List* files;
};

struct GobFile {
    GobFileHeader* header;
    GobArchive* parent;
};

OptionalPtr* gobOpenArchive(char* filename) {
    OptionalPtr* optionalFile = fileOpen(filename, "rb");
    if(optionalIsEmpty(optionalFile)) return optionalFile;

    return gobReadArchive(optionalGet(optionalFile));
}

void gobCloseArchiveFile(void* file) {
    GobFile* gobFile = file;
    memoryRelease(gobFile->header);
    memoryRelease(gobFile);
}

int gobCloseArchive(GobArchive* archive) {
    FILE* stream = archive->stream;

    listForEach(archive->files, &gobCloseArchiveFile);
    listDelete(archive->files);
    memoryRelease(archive->headers);
    memoryRelease(archive);
    
    return fclose(stream);
}

uint32_t gobCountFiles(GobArchive* archive){
    return listSize(archive->files);
}

List* gobListFiles(GobArchive* archive){
    return archive->files;
}

GobFile* gobGetFile(GobArchive* archive, char* file_name){
    for(int i = 0; i < listSize(archive->files); i++) {
        OptionalPtr* optionalGobFile = listGet(archive->files, i);
        GobFile* gob_file = optionalGet(optionalGobFile);
        if(strcmp(file_name, gob_file->header->name) == 0) return gob_file;
    }

    errno = ENOENT;
    return NULL;
}

char* gobGetFileName(GobFile* gob_file) {
    return gob_file->header->name;
}

InMemoryFile* gobReadFile(GobFile* gob_file){
    GobArchive* archive = gob_file->parent;
    FILE* stream = archive->stream;

    int failure = fseek(stream, gob_file->header->offset, SEEK_SET);
    if(failure) return NULL;

    char* content = memoryAllocateWithTag(gob_file->header->length, "odf/res/gob/GobFileContent");

    int objects = fread(content, gob_file->header->length, 1, stream);
    if(objects == 0) return NULL;
    content[gob_file->header->length-1] = (char) 0;

    OptionalPtr* optionalFile = memFileCreate(content, gob_file->header->length);
    if(optionalIsEmpty(optionalFile)) return NULL;

    return optionalGet(optionalFile);
}

void gobCloseFile(InMemoryFile* file) {
    inMemFileDelete(file);
}


int gobPrintArchive(GobArchive* archive){
    GobArchiveHeaders* headers = archive->headers;
    List* files = archive->files;
    uint32_t* magic = (uint32_t*) &(headers->magic);

    printf("Magic: 0x%x\n", *magic);
    printf("Directory offset: %d\n", headers->directoryOffset);
    printf("Number of Files: %d\n", listSize(files));
    printf("Files:\n");
    for(int i = 0; i < listSize(files); i++) {
        OptionalPtr* optionalGobFile = listGet(files, i);
        GobFile* gob_file = optionalGet(optionalGobFile);
        GobFileHeader* headers = gob_file->header;
        printf("\t%s (%d-%d)\n", headers->name, headers->offset, headers->offset + headers->length);
    }

    return 0;
}

OptionalPtr* gobReadArchive(FILE* stream) {
    GobArchive* archive = memoryAllocateWithTag(sizeof(GobArchive), "odf/res/gob/GobArchive");

    archive->stream = stream;

    OptionalPtr* optionalHeaders = fileReadStruct(stream, GOB_ARCHIVE_HEADER_FORMAT);
    if(optionalIsEmpty(optionalHeaders)) return optionalHeaders;
    archive->headers = optionalGet(optionalHeaders);

    uint32_t fileCount = gobReadArchiveFileCount(archive);
    if(fileCount == 0) return optionalEmpty("ogl/res/gob.c: Archive does not contain any files");

    OptionalPtr* optionalFiles = gobReadArchiveFiles(archive, fileCount);
    if(optionalIsEmpty(optionalFiles)) {
        memoryRelease(archive->headers);
        memoryRelease(archive);
        return (void*) optionalFiles;
    };
    archive->files = optionalGet(optionalFiles);

    return optionalOf(archive);
}

uint32_t gobReadArchiveFileCount(GobArchive* archive) {
    FILE* stream = archive->stream;

    int failure = fseek(stream, (long) archive->headers->directoryOffset, SEEK_SET);
    if(failure) return 0;

    OptionalPtr* optionalCount = fileReadStruct(stream, "%l4");
    if(optionalIsEmpty(optionalCount)) return 0;

    uint32_t* countPtr = optionalGet(optionalCount);
    uint32_t count = *countPtr;
    memoryRelease(countPtr);
    return count;
}

OptionalPtr* gobReadArchiveFiles(GobArchive* archive, uint32_t fileCount) {
    List* list = listCreate(fileCount);

    for(int i = 0; i < fileCount; i++) {

        OptionalPtr* optFile = gobReadArchiveFile(archive);

        if(optionalIsEmpty(optFile)) {
            for(int j = 0; j < i; j++) {
                OptionalPtr* item = listGet(list, j);
                memoryRelease(optionalGet(item));
            }

            listDelete(list);
            return optFile;
        }

        GobFile* file = memoryAllocateWithTag(sizeof(GobFile), "odf/res/gob/GobFile");
        file->header = optionalGet(optFile);
        file->parent = archive;

        OptionalPtr* opt = listPut(list, i, file);
        if(optionalIsEmpty(opt)) return opt;
        memoryRelease(opt);
    }

    return optionalOf(list);
}


OptionalPtr* gobReadArchiveFile(GobArchive* archive) {
    FILE* stream = archive->stream;
    return fileReadStruct(stream, GOB_FILE_HEADER_FORMAT);
}
