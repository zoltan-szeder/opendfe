#ifndef DISPLAY_MODLEL_H
#define DISPLAY_MODLEL_H
#include "types.h"
#include "dgl/texture.h"

typedef struct Model Model;

Model* dglModelCreate(float*, uint32,  uint32*, uint32);
void dglModelDelete(Model* model);

void dglModelBindTexture(Model* model, DglTexture* texture);

void dglModelDraw(Model* model);

#endif
