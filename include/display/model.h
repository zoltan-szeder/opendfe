#ifndef DISPLAY_MODLEL_H
#define DISPLAY_MODLEL_H
#include "types.h"

typedef struct Model Model;

Model* modelCreate(float*, uint32,  uint32*, uint32);
void modelDelete(Model* model);

void modelDraw(Model* model);

#endif
