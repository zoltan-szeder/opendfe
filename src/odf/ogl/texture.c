#include <GL/glew.h>
#include <stdlib.h>
#include "odf/ogl/texture.h"
#include "odf/sys/memory.h"

struct DglTexture {
    unsigned int id;
};

#define ROTCC90(NAME, TYPE) \
    static TYPE* NAME(TYPE* data, uint32_t w, uint32_t h) { \
        TYPE* rotatedImage = memoryAllocate(w*h*sizeof(TYPE)); \
        for(uint32_t i1 = 0; i1 <  w*h; i1++) { \
            uint32_t x = i1/h; \
            uint32_t y = i1%h; \
            uint32_t i2 = x + w*y; \
            rotatedImage[i2] = data[i1]; \
        } \
        return rotatedImage; \
    }

ROTCC90(rotcc90uc1, uint8_t)
ROTCC90(rotcc90uc2, ucvec2)
ROTCC90(rotcc90uc3, ucvec3)
ROTCC90(rotcc90uc4, ucvec4)

static uint8_t* dglNormalize(uint8_t* data, uint32_t width, uint32_t height, uint8_t channels);

GLint dglGetImageFormat(Image8Bit* img) {
    if(img->channels == 4)
        return GL_RGBA;
    else if(img->channels == 3)
        return GL_RGB;
    else
        return GL_RED;
}

DglTexture* dglTextureCreate(Image8Bit* img){
    DglTexture* tx = memoryAllocateWithTag(sizeof(DglTexture), "odf/ogl/texture/DglTexture");
    if(tx == NULL) {
        return NULL;
    }

    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format = dglGetImageFormat(img);

    uint32_t width = img->width;
    uint32_t height = img->height;
    uint32_t channels = img->channels;

    uint8_t* data = dglNormalize(img->data, width, height, channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    memoryRelease(data);

    tx->id = id;
    return tx;
}

void dglTextureDelete(DglTexture* texture) {
    unsigned int id = texture->id;
    glDeleteTextures(1, &id);
    memoryRelease(texture);
}


void dglTextureUse(DglTexture* texture){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

static uint8_t* dglNormalize(uint8_t* data, uint32_t width, uint32_t height, uint8_t channels) {
    if(channels == 4)
        return (uint8_t*) rotcc90uc4((ucvec4*) data, width, height);
    else if(channels == 3)
        return (uint8_t*) rotcc90uc3((ucvec3*) data, width, height);
    else if(channels == 2)
        return (uint8_t*) rotcc90uc2((ucvec2*) data, width, height);
    return rotcc90uc1(data, width, height);
}
