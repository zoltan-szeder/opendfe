#include "../headers/mob.h"
#include "../headers/types.h"
#include "../headers/mob.h"
#include <stdlib.h>
#include <string.h>

MOB_ARCHIVE* create_mob_archive_instance();
int mob_archive_fread(MOB_ARCHIVE*, FILE*);

int fread_uint32(uint32* number, FILE* stream);

struct mob_archive {
    char magic[4];
    uint32 directory_offset;
    uint32 file_count;

    MOB_FILE* files;

    FILE* stream;
};

struct mob_file {
    uint32 offset;
    uint32 length;
    char name[13];

    MOB_ARCHIVE* parent;
};

MOB_ARCHIVE* mob_archive_fopen(char* filename) {
    FILE* mob_file = fopen(filename, "rb");
    if(NULL == mob_file) return NULL;

    MOB_ARCHIVE* mob_archive = malloc(sizeof(MOB_ARCHIVE));
    if(NULL == mob_archive) return NULL;

    mob_archive_fread(mob_archive, mob_file);

    return mob_archive;
}

int mob_archive_fclose(MOB_ARCHIVE* mob_archive) {
    FILE* stream = mob_archive->stream;

    free(mob_archive->files);
    free(mob_archive);
    
    return fclose(stream);
}

uint32 mob_count_files(MOB_ARCHIVE* mob_archive){
    return mob_archive->file_count;
}

MOB_FILE* mob_list_files(MOB_ARCHIVE* mob_archive){
    return mob_archive->files;
}

MOB_FILE* mob_get_file(MOB_ARCHIVE* mob_archive, char* file_name){
    for(int i = 0; i < mob_archive->file_count; i++) {
        MOB_FILE* mob_file = &(mob_archive->files[i]);
        if(strcmp(file_name, mob_file->name) == 0) return mob_file;
    }
    return NULL;
}

IN_MEM_FILE* mob_read_file(MOB_FILE* mob_file){
    MOB_ARCHIVE* mob_archive = mob_file->parent;
    FILE* stream = mob_archive->stream;

    int failure = fseek(stream, mob_file->offset, SEEK_SET);
    if(failure) return NULL;

    char* content = malloc(mob_file->length + 1);
    if(!content) return NULL;

    int objects = fread(content, mob_file->length, 1, stream);
    if(objects == 0) return NULL;
    content[mob_file->length] = (char) 0;

    IN_MEM_FILE* file = malloc(sizeof(IN_MEM_FILE));
    if(!file) return NULL;

    file->content = content;
    file->length = mob_file->length;

    return file;
}

void mob_close_file(IN_MEM_FILE* file) {
    free(file->content);
    free(file);
}


int print_mob_archive(MOB_ARCHIVE* mob_archive){
    uint32* magic = (uint32*) &(mob_archive->magic);

    printf("Magic: 0x%x\n", *magic);
    printf("Directory offset: %d\n", mob_archive->directory_offset);
    printf("Number of Files: %d\n", mob_archive->file_count);
    printf("Files:\n");
    for(int i = 0; i < mob_archive->file_count; i++) {
        MOB_FILE* mob_file = &(mob_archive->files[i]);
        printf("\t%s (%d-%d)\n", mob_file->name, mob_file->offset, mob_file->offset + mob_file->length);
    }

    return 0;
}

int mob_file_fread(MOB_FILE* mob_file, FILE* stream) {
    int objects = fread(mob_file, 2*sizeof(uint32) + 13*sizeof(char), 1, stream);
    if(!objects) return 0;

    return 1;
}

int mob_archive_fread(MOB_ARCHIVE* mob_archive, FILE* stream) {
    mob_archive->stream = stream;

    int objects = 0;
    objects = fread(mob_archive, sizeof(uint32), 2, stream);
    if(!objects) return 0;

    int failure = fseek(stream, (long) mob_archive->directory_offset, SEEK_SET);
    if(failure) return 0;
    objects = fread_uint32(&(mob_archive->file_count), stream);
    if(!objects) return 0;

    mob_archive->files = malloc(mob_archive->file_count*sizeof(MOB_FILE));
    if(!mob_archive->files) return 0;
    for(int i = 0; i < mob_archive->file_count; i++) {
        MOB_FILE* mob_file = &(mob_archive->files[i]);
        mob_file->parent = mob_archive;
        objects = mob_file_fread(mob_file, stream);
        if(!objects) return 0;
    }

    return 1;
}

int fread_uint32(uint32* number, FILE* stream) {
    return fread(number, 4, 1, stream);
}
