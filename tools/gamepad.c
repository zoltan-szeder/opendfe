#include <stdio.h>
#include <stdlib.h>

#include "odf/ctrl/glfw/gamepad.h"
#include "odf/ctrl/ingameevent.h"

#include "odf/sys/gc.h"
#include "odf/sys/memory.h"
#include "odf/sys/optional.h"

#include "odf/ogl/main.h"
#include "odf/ogl/model.h"
#include "odf/ogl/texture.h"

int main(int argc, char** argv) {
    glfwInit();

    GameControl gameControl;
    gameControl.shouldExit = false;

    ctrlGlfwGamepadInit();
    while(!gameControl.shouldExit)
    {
        ctrlGlfwGamepadProcessAllIngameEvents();

        ListOf(CtrlEvent*)* events = ctrlInGameEventFetchAll();
        for(size_t i = 0; i < listSize(events); i++) {
            CtrlEvent* event = optionalGet(listGet(events, i));
            printf(
                "%s: %s - %g\n",
                ctrlGetCtrlActionTypeName(event->type),
                ctrlGetCtrlActionName(event->action),
                event->value
            );
            if(event->type == CTRL_FEAT_PAUSE) {
                gameControl.shouldExit = true;
            }
        }
        fflush(stdout);
        ctrlInGameEventReleaseAll(events);
    }

    if(!memoryIsEmpty()) {
        memoryDump(false);
        memoryReleaseAll();
    }

    return 0;
}
