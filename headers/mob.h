#ifndef MOB_H
#define MOB_H

#include <stdio.h>
#include <stddef.h>
#include "types.h"

typedef struct mob_archive MOB_ARCHIVE;
typedef struct mob_file MOB_FILE;
typedef struct {
    uint32 length;
    char* content;
} IN_MEM_FILE;

MOB_ARCHIVE* mob_archive_fopen(char* file_name);
int mob_archive_fclose(MOB_ARCHIVE*);

uint32 mob_count_files(MOB_ARCHIVE*);
MOB_FILE* mob_list_files(MOB_ARCHIVE*);
MOB_FILE* mob_get_file(MOB_ARCHIVE*, char*);

IN_MEM_FILE* mob_read_file(MOB_FILE*);
void mob_close_file(IN_MEM_FILE*);

int print_mob_archive(MOB_ARCHIVE*);
#endif
