#include "odf/ctrl/glfw/gamepad.h"
#include "odf/ctrl/ingameevent.h"
#include "odf/sys/log.h"

#define CTRL_GLFW_GAMEPAD_BUTTON_COUNT 15
#define CTRL_GLFW_GAMEPAD_AXIS_COUNT 6

#define ERROR = 0.01f

static uint8_t buttonStates[CTRL_GLFW_GAMEPAD_BUTTON_COUNT];
static float axisStates[CTRL_GLFW_GAMEPAD_AXIS_COUNT];

static bool gamepadButtonIsReleased(const GLFWgamepadstate* const state, uint8_t index);
static bool gamepadButtonIsPressed(const GLFWgamepadstate* const state, uint8_t index);
static CtrlActionType mapButtonToAction(uint8_t buttonIndex);

static bool gamepadAxisHasChanged(const GLFWgamepadstate* const state, uint8_t index);
static CtrlActionType mapAxisToAction(uint8_t buttonIndex, float value);
static bool almostEquals(float a, float b);

void ctrlGlfwGamepadInit() {
    for(uint8_t i = 0; i < CTRL_GLFW_GAMEPAD_BUTTON_COUNT; i++) {
        buttonStates[i] = GLFW_RELEASE;
    }

    for(uint8_t i = 0; i < CTRL_GLFW_GAMEPAD_AXIS_COUNT; i++) {
        axisStates[i] = 0.0f;
    }
    axisStates[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
    axisStates[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
}

void ctrlGlfwGamepadProcessAllIngameEvents() {
    GLFWgamepadstate state;
    for(int js = GLFW_JOYSTICK_1; js <= GLFW_JOYSTICK_16; js++) {
        if(glfwGetGamepadState(js, &state)) {
            ctrlGlfwGamepadProcessIngameEvents(js, &state);
            break;
        }
    }
}

void ctrlGlfwGamepadProcessIngameEvents(uint8_t gamepadNum, const GLFWgamepadstate* const state) {
    for(uint8_t i = 0; i < CTRL_GLFW_GAMEPAD_BUTTON_COUNT; i++) {
        if(gamepadButtonIsReleased(state, i)) {
            logTrace("Gamepad button #%d is released", i);
            ctrlInGameEventRegister(mapButtonToAction(i), CTRL_RELEASE, 0.0f);
        } else if(gamepadButtonIsPressed(state, i)) {
            logTrace("Gamepad button #%d is pressed", i);
            ctrlInGameEventRegister(mapButtonToAction(i), CTRL_PRESS, 1.0f);
        }
        buttonStates[i] = state->buttons[i];
    }
    for(uint8_t i = 0; i < CTRL_GLFW_GAMEPAD_AXIS_COUNT; i++) {
        if(gamepadAxisHasChanged(state, i)) {
            float value = state->axes[i];
            float oldValue = axisStates[i];
            if(i == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER || i == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) {
                oldValue = (oldValue + 1.0f) / 2.0f;
                value = (value + 1.0f) / 2.0f;
                if(almostEquals(oldValue, 0) && !almostEquals(value, 0)) {
                    ctrlInGameEventRegister(mapAxisToAction(i, value), CTRL_PRESS, 1.0);
                } else if (!almostEquals(oldValue, 0) && almostEquals(value, 0)) {
                    ctrlInGameEventRegister(mapAxisToAction(i, value), CTRL_RELEASE, 0.0f);
                }
            } else {
                logTrace("Gamepad axis #%d changed from %g to %g", i, oldValue, value);

                if(almostEquals(value, 0.0f)) {
                    ctrlInGameEventRegister(mapAxisToAction(i, oldValue), CTRL_RELEASE, 0.0f);
                } else {
                    if(oldValue * value < 0.0f){
                        ctrlInGameEventRegister(mapAxisToAction(i, oldValue), CTRL_RELEASE, 0.0f);
                    }
                    float absValue = value < 0 ? -value : value;
                    ctrlInGameEventRegister(mapAxisToAction(i, value), CTRL_PRESS, absValue);
                }
            }
        }
        axisStates[i] = state->axes[i];
    }
}


static CtrlActionType mapButtonToAction(uint8_t buttonIndex) {
    switch(buttonIndex) {
        case GLFW_GAMEPAD_BUTTON_SOUTH: return CTRL_MOV_ACTIVATE;
        case GLFW_GAMEPAD_BUTTON_EAST: return CTRL_MOV_RUN;
        case GLFW_GAMEPAD_BUTTON_WEST: return CTRL_MOV_CROUCH;
        case GLFW_GAMEPAD_BUTTON_NORTH: return CTRL_MOV_JUMP;

        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return CTRL_WEAP_PREV;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return CTRL_WEAP_NEXT;

        case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return CTRL_FEAT_MAP;
        case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return CTRL_FEAT_MAP_PDA;

        case GLFW_GAMEPAD_BUTTON_DPAD_UP: return CTRL_FEAT_HEAD_LAMP;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return CTRL_FEAT_GAS_MASK;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return CTRL_FEAT_CLEATS;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return CTRL_FEAT_GOOGLES;

        case GLFW_GAMEPAD_BUTTON_START: return CTRL_FEAT_PAUSE;
    }
    return 0;
}

static bool gamepadButtonIsReleased(const GLFWgamepadstate* const state, uint8_t index) {
    uint8_t previousState = buttonStates[index];
    uint8_t currentState = state->buttons[index];

    return previousState == GLFW_PRESS && currentState == GLFW_RELEASE;
}


static bool almostEquals(float a, float b) {
    float error = 0.01f;
    return b - error < a && a < b + error;
}

static bool gamepadButtonIsPressed(const GLFWgamepadstate* const state, uint8_t index) {
    uint8_t previousState = buttonStates[index];
    uint8_t currentState = state->buttons[index];

    return previousState == GLFW_RELEASE && currentState == GLFW_PRESS;
}

static bool gamepadAxisHasChanged(const GLFWgamepadstate* const state, uint8_t index) {
    return !almostEquals(axisStates[index], state->axes[index]);
}
static CtrlActionType mapAxisToAction(uint8_t buttonIndex, float value) {
    switch(buttonIndex) {
        case GLFW_GAMEPAD_AXIS_LEFT_X: return (value > 0 ? CTRL_MOV_STRAFE_RIGHT : CTRL_MOV_STRAFE_LEFT); break;
        case GLFW_GAMEPAD_AXIS_LEFT_Y: return (value > 0 ? CTRL_MOV_BACKWARD : CTRL_MOV_FORWARD); break;
        case GLFW_GAMEPAD_AXIS_RIGHT_X: return (value > 0 ? CTRL_MOV_TURN_RIGHT : CTRL_MOV_TURN_LEFT); break;
        case GLFW_GAMEPAD_AXIS_RIGHT_Y: return (value > 0 ? CTRL_MOV_LOOK_DOWN : CTRL_MOV_LOOK_UP); break;
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return CTRL_WEAP_SHOOT_SEC; break;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return CTRL_WEAP_SHOOT_PRI; break;
    }
    return 0;
}
