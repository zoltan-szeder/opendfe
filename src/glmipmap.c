#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "glmipmap.h"

Mipmap* openMipmap(char* filePath) {
    Mipmap* mipmap = malloc(sizeof(Mipmap));

    mipmap->data = stbi_load(
        filePath,
        &(mipmap->width),
        &(mipmap->height),
        &(mipmap->channels),
        0
    );

    return mipmap;
}



void closeMipmap(Mipmap* mipmap) {
    stbi_image_free(mipmap->data);

    free(mipmap);
}
