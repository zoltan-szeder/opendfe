#ifndef ODF_OGL_TEXTURE_H
#define ODF_OGL_TEXTURE_H

#include "odf/sys/img.h"

typedef struct DglTexture DglTexture;

DglTexture* dglTextureCreate(Image8Bit*);
void dglTextureDelete(DglTexture*);

void dglTextureUse(DglTexture*);

#endif
