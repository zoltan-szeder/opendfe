#ifndef ODF_OGL_DYNTEX_H
#define ODF_OGL_DYNTEX_H

#include "odf/sys/types/list.h"
#include "odf/ogl/texture.h"


typedef struct OGLDynamicTexture OGLDynamicTexture;

OGLDynamicTexture* oglDtCreateTexture();
void oglDtAddTexture(OGLDynamicTexture* dyntex, DglTexture*);

DglTexture* oglDtGetCurrentTexture(OGLDynamicTexture*);
void oglDtSwitchTexture(OGLDynamicTexture*);

void oglDtDeleteTextures(OGLDynamicTexture*);


#endif
