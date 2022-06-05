#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "glmipmap.h"

unsigned int load2dTextureFromMipMap(Mipmap* mipmap);

unsigned int load2dTexture(int width, int height, unsigned char* data);

#endif
