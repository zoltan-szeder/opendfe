#include <stdio.h>
#include <stdlib.h>

char* fileReadAll(char* filePath) {
    FILE* stream = fopen(filePath, "rb");
    if(stream == NULL) return NULL;

    fseek(stream, 0, SEEK_END);
    long length = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    if(length == -1L) return NULL;

    char* content = malloc(length);
    fread(content, length, 1, stream);
    content[length-1] = (char) 0;
    fclose(stream);

    return content;
}

