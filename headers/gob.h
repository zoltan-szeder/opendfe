#ifndef GOB_H
#define GOB_H

#include <stdio.h>
#include <stddef.h>
#include "types.h"

typedef struct gob_archive GOB_ARCHIVE;
typedef struct gob_file GOB_FILE;
typedef struct {
    uint32 length;
    char* content;
} IN_MEM_FILE;

GOB_ARCHIVE* gob_archive_fopen(char* file_name);
int gob_archive_fclose(GOB_ARCHIVE*);

uint32 gob_count_files(GOB_ARCHIVE*);
GOB_FILE* gob_list_files(GOB_ARCHIVE*);
GOB_FILE* gob_get_file(GOB_ARCHIVE*, char*);

IN_MEM_FILE* gob_read_file(GOB_FILE*);
void gob_close_file(IN_MEM_FILE*);

int print_gob_archive(GOB_ARCHIVE*);
#endif
