#ifndef CORE_GAMESTATE_H
#define CORE_GAMESTATE_H

#include <stdint.h>

#include "odf/ogl/model.h"

typedef struct {
    uint32_t texture;
    Model* model;
} GameState;

#endif
