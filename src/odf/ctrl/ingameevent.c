#include "odf/ctrl/ingameevent.h"
#include "odf/sys/list.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

static ListOf(CtrlEvent*)* EVENT_QUEUE = NULL;

void ctrlInGameEventRegister(CtrlActionType type, CtrlAction action, float value) {
    if(EVENT_QUEUE == NULL)
        EVENT_QUEUE = listCreate(0);

    CtrlEvent* event = memoryAllocate(sizeof(CtrlEvent));
    event->action = action;
    event->type = type;
    event->value = value;

    listAppend(EVENT_QUEUE, event);
}

ListOf(CtrlEvent*)* ctrlInGameEventFetchAll(){
    if(EVENT_QUEUE == NULL)
        return listCreate(0);

    List* eventQueue = EVENT_QUEUE;
    EVENT_QUEUE = NULL;
    return eventQueue;
}

void ctrlInGameEventReleaseAll(ListOf(CtrlEvent*)* events){
    for(size_t i = 0; i < listSize(events); i++) {
        CtrlEvent* event = optionalGet(listGet(events, i));
        memoryRelease(event);
    }
    listDelete(events);
}


const char* ctrlGetCtrlActionTypeName(CtrlActionType type) {
    switch(type) {

        case CTRL_MOV_FORWARD: return "Move forward"; break;
        case CTRL_MOV_BACKWARD: return "Move backward"; break;
        case CTRL_MOV_STRAFE_LEFT: return "Strafe left"; break;
        case CTRL_MOV_STRAFE_RIGHT: return "Strafe right"; break;

        case CTRL_MOV_LOOK_UP: return "Look up"; break;
        case CTRL_MOV_LOOK_DOWN: return "Look down"; break;
        case CTRL_MOV_TURN_LEFT: return "Look left"; break;
        case CTRL_MOV_TURN_RIGHT: return "Look right"; break;

        case CTRL_MOV_ACTIVATE: return "Activate"; break;
        case CTRL_MOV_CROUCH: return "Crouch"; break;
        case CTRL_MOV_JUMP: return "Jump"; break;
        case CTRL_MOV_RUN: return "Run"; break;
        case CTRL_MOV_SLOW: return "Slow"; break;

        case CTRL_WEAP_SHOOT_PRI: return "Shoot primary"; break;
        case CTRL_WEAP_SHOOT_SEC: return "Shoot secondary"; break;
        case CTRL_WEAP_NEXT: return "Next weapon"; break;
        case CTRL_WEAP_PREV: return "Previous weapon"; break;
        case CTRL_WEAP_TOGGLE: return "Holster weapon"; break;

        case CTRL_WEAP_FIST: return "Use fist"; break;
        case CTRL_WEAP_PISTOL: return "Use pistol"; break;
        case CTRL_WEAP_RIFLE: return "Use rifle"; break;
        case CTRL_WEAP_DETONATOR: return "Use detonator"; break;
        case CTRL_WEAP_REPEATER: return "Use repeater"; break;
        case CTRL_WEAP_FUSION: return "Use fusion"; break;
        case CTRL_WEAP_MINE: return "Use mine"; break;
        case CTRL_WEAP_MORTAR: return "Use mortar"; break;
        case CTRL_WEAP_CONCUSSION: return "Use concussion"; break;
        case CTRL_WEAP_CANNON: return "Use cannon"; break;

        case CTRL_FEAT_MAP: return "Use map"; break;
        case CTRL_FEAT_MAP_ZOOM_IN: return "Zoom in"; break;
        case CTRL_FEAT_MAP_ZOOM_OUT: return "Zoom out"; break;
        case CTRL_FEAT_MAP_PDA: return "Use PDA"; break;
        case CTRL_FEAT_GOOGLES: return "Use googles"; break;
        case CTRL_FEAT_HEAD_LAMP: return "Use head lamp"; break;
        case CTRL_FEAT_CLEATS: return "Use cleats"; break;
        case CTRL_FEAT_HEAD_WAVE: return "Wave head"; break;
        case CTRL_FEAT_GAS_MASK: return "Use gas mask"; break;
        case CTRL_FEAT_HUD: return "Display HUD"; break;
        case CTRL_FEAT_PAUSE: return "Pause"; break;
        case CTRL_FEAT_AUTOMOUNT: return "Mount automatically"; break;
    }

    return "UNKNOWN";
}

const char* ctrlGetCtrlActionName(CtrlAction action) {
    switch(action) {
        case CTRL_PRESS: return "pressed";
        case CTRL_REPEAT: return "repeated";
        case CTRL_RELEASE: return "released";
    }

    return "UNKNOWN";
}
