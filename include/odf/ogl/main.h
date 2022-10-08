#ifndef ODF_OGL_MAIN_H
#define ODF_OGL_MAIN_H

#include "core/gamestate.h"
#include "core/control.h"

typedef struct Display Display;

Display* dglCreateDisplay();
void dglDestroyDisplay(Display*);

GameControl* dglReadInputs(Display*, GameControl*);

void dglDraw(Display*, GameState*);

#endif
