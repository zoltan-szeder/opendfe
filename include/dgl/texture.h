#ifndef DISPLAY_TEXTURE_H
#define DISPLAY_TEXTURE_H

#include "img.h"

typedef struct DglTexture DglTexture;

DglTexture* dglTextureCreate(Image8Bit*);
void dglTextureDelete(DglTexture*);

void dglTextureUse(DglTexture*);

#endif
