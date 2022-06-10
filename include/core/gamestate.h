#ifndef CORE_GAMESTATE_H
#define CORE_GAMESTATE_H

#include "dgl/model.h"

typedef struct {
    uint32 texture;
    Model* model;
} GameState;

#endif
