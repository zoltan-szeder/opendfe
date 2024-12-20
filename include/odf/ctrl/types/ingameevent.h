#ifndef ODF_CTRL_TYPES_IN_GAME_EVENT_H
#define ODF_CTRL_TYPES_IN_GAME_EVENT_H

#include <stdint.h>

typedef enum {
    CTRL_MOV_FORWARD,
    CTRL_MOV_BACKWARD,
    CTRL_MOV_STRAFE_LEFT,
    CTRL_MOV_STRAFE_RIGHT,

    CTRL_MOV_LOOK_UP,
    CTRL_MOV_LOOK_DOWN,
    CTRL_MOV_TURN_LEFT,
    CTRL_MOV_TURN_RIGHT,

    CTRL_MOV_ACTIVATE,
    CTRL_MOV_CROUCH,
    CTRL_MOV_JUMP,
    CTRL_MOV_RUN,
    CTRL_MOV_SLOW,

    CTRL_WEAP_SHOOT_PRI,
    CTRL_WEAP_SHOOT_SEC,
    CTRL_WEAP_NEXT,
    CTRL_WEAP_PREV,
    CTRL_WEAP_TOGGLE,

    CTRL_WEAP_FIST,
    CTRL_WEAP_PISTOL,
    CTRL_WEAP_RIFLE,
    CTRL_WEAP_DETONATOR,
    CTRL_WEAP_REPEATER,
    CTRL_WEAP_FUSION,
    CTRL_WEAP_MINE,
    CTRL_WEAP_MORTAR,
    CTRL_WEAP_CONCUSSION,
    CTRL_WEAP_CANNON,

    CTRL_FEAT_MAP,
    CTRL_FEAT_MAP_ZOOM_IN,
    CTRL_FEAT_MAP_ZOOM_OUT,
    CTRL_FEAT_MAP_PDA,
    CTRL_FEAT_GOOGLES,
    CTRL_FEAT_HEAD_LAMP,
    CTRL_FEAT_CLEATS,
    CTRL_FEAT_HEAD_WAVE,
    CTRL_FEAT_GAS_MASK,
    CTRL_FEAT_HUD,
    CTRL_FEAT_PAUSE,
    CTRL_FEAT_AUTOMOUNT,
} CtrlActionType;

typedef enum {
    CTRL_PRESS,
    CTRL_REPEAT,
    CTRL_RELEASE,
} CtrlAction;

typedef struct {
    CtrlAction action;
    CtrlActionType type;
    float value;
} CtrlEvent;

#endif
