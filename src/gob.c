#include "../headers/gob.h"
#include "../headers/types.h"
#include <stdlib.h>
#include <string.h>

GOB_ARCHIVE* create_gob_archive_instance();
int gob_archive_fread(GOB_ARCHIVE*, FILE*);

int fread_uint32(uint32* number, FILE* stream);

struct gob_archive {
    char magic[4];
    uint32 directory_offset;
    uint32 file_count;

    GOB_FILE* files;

    FILE* stream;
};

struct gob_file {
    uint32 offset;
    uint32 length;
    char name[13];

    GOB_ARCHIVE* parent;
};

GOB_ARCHIVE* gob_archive_fopen(char* filename) {
    FILE* gob_file = fopen(filename, "rb");
    if(NULL == gob_file) return NULL;

    GOB_ARCHIVE* gob_archive = malloc(sizeof(GOB_ARCHIVE));
    if(NULL == gob_archive) return NULL;

    gob_archive_fread(gob_archive, gob_file);

    return gob_archive;
}

int gob_archive_fclose(GOB_ARCHIVE* gob_archive) {
    FILE* stream = gob_archive->stream;

    free(gob_archive->files);
    free(gob_archive);
    
    return fclose(stream);
}

uint32 gob_count_files(GOB_ARCHIVE* gob_archive){
    return gob_archive->file_count;
}

GOB_FILE* gob_list_files(GOB_ARCHIVE* gob_archive){
    return gob_archive->files;
}

GOB_FILE* gob_get_file(GOB_ARCHIVE* gob_archive, char* file_name){
    for(int i = 0; i < gob_archive->file_count; i++) {
        GOB_FILE* gob_file = &(gob_archive->files[i]);
        if(strcmp(file_name, gob_file->name) == 0) return gob_file;
    }
    return NULL;
}

IN_MEM_FILE* gob_read_file(GOB_FILE* gob_file){
    GOB_ARCHIVE* gob_archive = gob_file->parent;
    FILE* stream = gob_archive->stream;

    int failure = fseek(stream, gob_file->offset, SEEK_SET);
    if(failure) return NULL;

    char* content = malloc(gob_file->length);
    if(!content) return NULL;

    int objects = fread(content, gob_file->length, 1, stream);
    if(objects == 0) return NULL;
    content[gob_file->length-1] = (char) 0;

    IN_MEM_FILE* file = malloc(sizeof(IN_MEM_FILE));
    if(!file) return NULL;

    file->content = content;
    file->length = gob_file->length;

    return file;
}

void gob_close_file(IN_MEM_FILE* file) {
    free(file->content);
    free(file);
}


int print_gob_archive(GOB_ARCHIVE* gob_archive){
    uint32* magic = (uint32*) &(gob_archive->magic);

    printf("Magic: 0x%x\n", *magic);
    printf("Directory offset: %d\n", gob_archive->directory_offset);
    printf("Number of Files: %d\n", gob_archive->file_count);
    printf("Files:\n");
    for(int i = 0; i < gob_archive->file_count; i++) {
        GOB_FILE* gob_file = &(gob_archive->files[i]);
        printf("\t%s (%d-%d)\n", gob_file->name, gob_file->offset, gob_file->offset + gob_file->length);
    }

    return 0;
}

int gob_file_fread(GOB_FILE* gob_file, FILE* stream) {
    int objects = fread(gob_file, 2*sizeof(uint32) + 13*sizeof(char), 1, stream);
    if(!objects) return 0;

    return 1;
}

int gob_archive_fread(GOB_ARCHIVE* gob_archive, FILE* stream) {
    gob_archive->stream = stream;

    int objects = 0;
    objects = fread(gob_archive, sizeof(uint32), 2, stream);
    if(!objects) return 0;

    int failure = fseek(stream, (long) gob_archive->directory_offset, SEEK_SET);
    if(failure) return 0;
    objects = fread_uint32(&(gob_archive->file_count), stream);
    if(!objects) return 0;

    gob_archive->files = malloc(gob_archive->file_count*sizeof(GOB_FILE));
    if(!gob_archive->files) return 0;
    for(int i = 0; i < gob_archive->file_count; i++) {
        GOB_FILE* gob_file = &(gob_archive->files[i]);
        gob_file->parent = gob_archive;
        objects = gob_file_fread(gob_file, stream);
        if(!objects) return 0;
    }

    return 1;
}

int fread_uint32(uint32* number, FILE* stream) {
    return fread(number, 4, 1, stream);
}
