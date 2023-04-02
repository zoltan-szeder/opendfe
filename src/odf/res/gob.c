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

static OptionalPtr* gobReadArchive(FILE*);
static OptionalPtr* gobReadArchiveFile(GobArchive*);
static OptionalPtr* gobReadArchiveFiles(GobArchive*, uint32_t fileCount);
static uint32_t gobReadArchiveFileCount(FILE* stream, GobArchiveHeaders* headers);

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
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        FILE*, file, fileOpen(filename, "rb")
    );

    return gobReadArchive(file);
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

size_t gobCountFiles(GobArchive* archive){
    return listSize(archive->files);
}

List* gobListFiles(GobArchive* archive){
    return archive->files;
}

OptionalPtr* gobGetFile(GobArchive* archive, char* file_name){
    for(size_t i = 0; i < listSize(archive->files); i++) {
        OptionalPtr* optionalGobFile = listGet(archive->files, i);
        GobFile* gob_file = optionalGet(optionalGobFile);
        if(strcmp(file_name, gob_file->header->name) == 0)
            return optionalOf(gob_file);
    }

    return optionalEmpty("gobGetFile - Could not find \"%s\" in GOB archive", file_name);
}

char* gobGetFileName(GobFile* gob_file) {
    return gob_file->header->name;
}

OptionalPtr* gobReadFile(GobFile* gob_file){
    GobArchive* archive = gob_file->parent;
    FILE* stream = archive->stream;

    int failure = fseek(stream, gob_file->header->offset, SEEK_SET);
    if(failure)
        return optionalEmpty("gobReadFile - Could not find beginning of GOB archive content");

    char* content = memoryAllocateWithTag(gob_file->header->length, "odf/res/gob/GobFileContent");

    int objects = fread(content, gob_file->header->length, 1, stream);
    if(objects == 0)
        return optionalEmpty(
            "gobReadFile - Could not read file GOB file \"%s\"",
            gobGetFileName(gob_file)
        );

    //content[gob_file->header->length-1] = (char) 0;

    return memFileCreate(content, gob_file->header->length);
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
    printf("Number of Files: %ld\n", listSize(files));
    printf("Files:\n");
    for(size_t i = 0; i < listSize(files); i++) {
        OptionalPtr* optionalGobFile = listGet(files, i);
        GobFile* gob_file = optionalGet(optionalGobFile);
        GobFileHeader* headers = gob_file->header;
        printf("\t%s (%d-%d)\n", headers->name, headers->offset, headers->offset + headers->length);
    }

    return 0;
}

static OptionalPtr* gobReadArchive(FILE* stream) {
    OPTIONAL_ASSIGN_OR_PASSTROUGH(
        GobArchiveHeaders*, headers, fileReadStruct(stream, GOB_ARCHIVE_HEADER_FORMAT)
    );

    uint32_t fileCount = gobReadArchiveFileCount(stream, headers);
    if(fileCount == 0) {
        memoryRelease(headers);
        return optionalEmpty("gobReadArchive - Archive is empty");
    }

    GobArchive* archive = memoryAllocateWithTag(sizeof(GobArchive), "odf/res/gob/GobArchive");
    archive->stream = stream;
    archive->headers = headers;

    OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
        List*, files, gobReadArchiveFiles(archive, fileCount),
        {
            memoryRelease(archive->headers);
            memoryRelease(archive);
        }
    );

    archive->files = files;

    return optionalOf(archive);
}

static uint32_t gobReadArchiveFileCount(FILE* stream, GobArchiveHeaders* headers) {
    int failure = fseek(stream, (long) headers->directoryOffset, SEEK_SET);
    if(failure) return 0;

    OPTIONAL_ASSIGN_OR_RETURN(
        uint32_t*, countPtr, fileReadStruct(stream, "%l4"),
        0
    )

    uint32_t count = *countPtr;
    memoryRelease(countPtr);
    return count;
}

static OptionalPtr* gobReadArchiveFiles(GobArchive* archive, uint32_t fileCount) {
    List* list = listCreate(fileCount);

    for(uint32_t i = 0; i < fileCount; i++) {
        OPTIONAL_ASSIGN_OR_CLEANUP_AND_PASS(
            GobFileHeader*, header, gobReadArchiveFile(archive),
            {
                for(uint32_t j = 0; j < i; j++) {
                    memoryRelease(optionalGet(listGet(list, j)));
                }

                listDelete(list);
            }
        )

        GobFile* file = memoryAllocateWithTag(sizeof(GobFile), "odf/res/gob/GobFile");
        file->header = header;
        file->parent = archive;

        listAppend(list, file);
    }

    return optionalOf(list);
}


static OptionalPtr* gobReadArchiveFile(GobArchive* archive) {
    FILE* stream = archive->stream;
    return fileReadStruct(stream, GOB_FILE_HEADER_FORMAT);
}
