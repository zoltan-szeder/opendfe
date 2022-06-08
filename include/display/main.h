#ifndef DISPLAY_MAIN_H
#define DISPLAY_MAIN_H
#include "core/gamestate.h"
#include "core/control.h"

typedef struct Display Display;

Display* dglCreateDisplay();
void dglDestroyDisplay(Display*);

GameControl* dglReadInputs(Display*, GameControl*);

void dglDraw(Display*, GameState*);

#endif
