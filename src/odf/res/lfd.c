#include "odf/res/lfd.h"
#include "odf/sys/file.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"
#include "odf/sys/list.h"
#include "odf/sys/inmemoryfile.h"

const char* LFD_CHUNK_FORMAT = "%c4 %c8 %l4";
typedef LfdChunk LfdHeader;

struct LfdArchive {
    LfdHeader* header;
    List* files;
    FILE* file;
};


struct LfdFile {
    LfdHeader* header;
    uint32 offset;
    FILE* file;
};

OptionalPtr* lfdReadLfdFileHeader(FILE* file);
void lfdPrintHeader(const char* prefix, LfdHeader* header);

OptionalPtr* lfdOpenArchive(char* file_name) {
    OptionalPtr* optFile = fileOpen(file_name, "rb");
    if(optionalIsEmpty(optFile)) return optFile;
    FILE* file = optionalGet(optFile);

    LfdArchive* archive = memoryAllocate(sizeof(LfdArchive));

    OptionalPtr* optHeader =  fileReadStruct(file, LFD_CHUNK_FORMAT);
    if(optionalIsEmpty(optHeader)) return optHeader;
    archive->header = optionalGet(optHeader);

    uint32 fileCount = lfdCountFiles(archive);
    archive->files = listCreate(fileCount);
    uint32 currentOffset = sizeof(LfdHeader) + fileCount*sizeof(LfdHeader);
    for(uint32 i = 0; i < fileCount; i++) {
        OptionalPtr* optFileHeader = fileReadStruct(file, LFD_CHUNK_FORMAT);
        if(optionalIsEmpty(optFileHeader)) return optFileHeader;

        LfdHeader* fileHeader = optionalGet(optFileHeader);
        LfdFile* lfdFile = memoryAllocate(sizeof(LfdFile));
        lfdFile->header = fileHeader;
        lfdFile->file = file;
        lfdFile->offset = currentOffset;
        currentOffset += fileHeader->length;

        listPut(archive->files, i, lfdFile);
    }

    return optionalOf(archive);
}

void lfdCloseArchive(LfdArchive* archive) {
    fclose(archive->file);

    List* files = archive->files;
    for(int i = 0; i < lfdCountFiles(archive); i++) {
        LfdFile* file = optionalGet(listGet(files, i));
        memoryRelease(file->header);
        memoryRelease(file);
    }
    listDelete(files);

    memoryRelease(archive->header);
    memoryRelease(archive);
}
void lfdPrintArchive(LfdArchive* archive) {
    lfdPrintHeader("", archive->header);
    printf("\n");
    uint32 fileCount = lfdCountFiles(archive);
    for(uint32 i = 0; i < fileCount; i++) {
        LfdFile* lfdFile =  optionalGet(listGet(archive->files, i));
        lfdPrintHeader("- ", lfdFile->header);
        printf("- Offset: %d\n", lfdFile->offset);
        printf("\n");
    }
}

void lfdPrintHeader(const char* prefix, LfdHeader* header) {
    printf("%sType: %.4s\n", prefix, header->type);
    printf("%sName: %.8s\n", prefix, header->name);
    printf("%sLength: %d\n", prefix, header->length);
}

void lfdPrintChunk(const char* prefix, LfdChunk* header) {
    printf("%sType: %.4s\n", prefix, header->type);
    printf("%sName: %.8s\n", prefix, header->name);
    printf("%sLength: %d\n", prefix, header->length);
}

uint32 lfdCountFiles(LfdArchive* archive) {
    return archive->header->length / 16;
}
List* lfdListFiles(LfdArchive* archive){
    return archive->files;
}

LfdFile* lfdGetFile(LfdArchive*, char*);
char* lfdGetFileName(LfdFile* file) {
    return file->header->name;
}

OptionalPtr* lfdReadFile(LfdFile* lfdFile) {
    FILE* file = lfdFile->file;
    uint32 length = lfdFile->header->length;

    fseek(file, lfdFile->offset, SEEK_SET);
    OptionalPtr* optContent = fileReadBytes(file, length);
    if(optionalIsEmpty(optContent)) return optContent;

    return memFileCreate(optionalGet(optContent), length);
}
void lfdCloseFile(InMemoryFile*);
