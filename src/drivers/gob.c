#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "drivers/gob.h"
#include "types.h"
#include "file.h"
#include "system/memory.h"
#include "system/list.h"

OptionalPtr* gobReadArchive(FILE*);
OptionalPtr* gobReadArchiveFile(GobArchive*);
OptionalPtr* gobReadArchiveFiles(GobArchive*, uint32 fileCount);
OptionalUInt32* gobReadArchiveFileCount(GobArchive* archive);

const char* GOB_ARCHIVE_HEADER_FORMAT = "%c4 %l4";
typedef struct GobArchiveHeaders {
    char magic[4];
    uint32 directoryOffset;
} GobArchiveHeaders;

struct GobArchive {
    FILE* stream;
    GobArchiveHeaders* headers;
    List* files;
};

const char* GOB_FILE_HEADER_FORMAT = "%l4 %l4 %c13";
typedef struct GobFileHeader {
    uint32 offset;
    uint32 length;
    char name[13];
} GobFileHeader;

struct GobFile {
    GobFileHeader* header;
    GobArchive* parent;
};

OptionalPtr* gobOpenArchive(char* filename) {
    OptionalPtr* optionalFile = fileOpen(filename, "rb");
    if(optionalIsEmpty(optionalFile)) return optionalFile;

    return gobReadArchive(optionalGet(optionalFile));
}

int gobCloseArchive(GobArchive* archive) {
    FILE* stream = archive->stream;

    listDelete(archive->files);
    free(archive);
    
    return fclose(stream);
}

uint32 gobCountFiles(GobArchive* archive){
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

    char* content = malloc(gob_file->header->length);
    if(!content) return NULL;

    int objects = fread(content, gob_file->header->length, 1, stream);
    if(objects == 0) return NULL;
    content[gob_file->header->length-1] = (char) 0;

    OptionalPtr* optionalFile = memFileCreate(content, gob_file->header->length);
    if(optionalIsEmpty(optionalFile)) return NULL;

    return optionalGet(optionalFile);
}

void gobCloseFile(InMemoryFile* file) {
    free(file->content);
    free(file);
}


int gobPrintArchive(GobArchive* archive){
    GobArchiveHeaders* headers = archive->headers;
    List* files = archive->files;
    uint32* magic = (uint32*) &(headers->magic);

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
    GobArchive* archive = memoryAllocate(sizeof(GobArchive));

    archive->stream = stream;

    OptionalPtr* optionalHeaders = fileReadStruct(stream, GOB_ARCHIVE_HEADER_FORMAT);
    if(optionalIsEmpty(optionalHeaders)) return optionalHeaders;
    archive->headers = optionalGet(optionalHeaders);

    OptionalUInt32* optionalFileCount = gobReadArchiveFileCount(archive);
    if(optionalIsEmpty(optionalFileCount)) return (void*) optionalFileCount;
    uint32 fileCount = optionalGetUInt32(optionalFileCount);

    OptionalPtr* optionalFiles = gobReadArchiveFiles(archive, fileCount);
    if(optionalIsEmpty(optionalFiles)) return (void*) optionalFiles;
    archive->files = optionalGet(optionalFiles);

    return optionalOf(archive);
}

OptionalUInt32* gobReadArchiveFileCount(GobArchive* archive) {
    FILE* stream = archive->stream;

    int failure = fseek(stream, (long) archive->headers->directoryOffset, SEEK_SET);
    if(failure) return optionalEmpty("gob.c:gobReadArchiveFileCount - Directory offset is over EOF");

    OptionalPtr* optionalCount = fileReadStruct(stream, "%l4");
    if(optionalIsEmpty(optionalCount)) return (void*) optionalCount;

    uint32* countPtr = optionalGet(optionalCount);
    uint32 count = *countPtr;
    memoryRelease(countPtr);
    return optionalUInt32(count);
}

OptionalPtr* gobReadArchiveFiles(GobArchive* archive, uint32 fileCount) {
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

        GobFile* file = memoryAllocate(sizeof(GobFile));
        file->header = optionalGet(optFile);
        file->parent = archive;

        listPut(list, i, file);
    }

    return optionalOf(list);
}


OptionalPtr* gobReadArchiveFile(GobArchive* archive) {
    FILE* stream = archive->stream;
    return fileReadStruct(stream, GOB_FILE_HEADER_FORMAT);
}
