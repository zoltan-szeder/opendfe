#ifndef ODF_OGL_MODEL_H
#define ODF_OGL_MODEL_H
#include "types.h"
#include "odf/ogl/texture.h"

typedef struct Model Model;

Model* dglModelCreate(float*, uint32,  uint32*, uint32);
void dglModelDelete(Model* model);

void dglModelBindTexture(Model* model, DglTexture* texture);

void dglModelDraw(Model* model);

#endif
