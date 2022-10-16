#include <GL/glew.h>
#include <stdlib.h>
#include "odf/ogl/texture.h"
#include "odf/sys/memory.h"

struct DglTexture {
    unsigned int id;
};

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format = dglGetImageFormat(img);
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->width, img->height, 0, format, GL_UNSIGNED_BYTE, img->data);
    glGenerateMipmap(GL_TEXTURE_2D);

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
