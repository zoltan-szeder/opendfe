#include "test_fixtures.h"
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "odf/ctrl/glfw/gamepad.h"
#include "odf/ctrl/ingameevent.h"
#include "odf/sys/optional.h"
#include "odf/sys/log.h"

GLFWgamepadstate gamepad;

ListOf(CtrlEvent*)* events;

static int setUp(void **state) {
    ctrlGlfwGamepadInit();
    for(uint8_t i = 0; i < 15; i++) {
        gamepad.buttons[i] = GLFW_RELEASE;
    }
    for(uint8_t i = 0; i < 6; i++) {
        gamepad.axes[i] = 0.0f;
    }
    gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
    gamepad.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
    return 0;
}

static int tearDown(void **state) {
    ctrlInGameEventReleaseAll(events);
    return 0;
}

static void press(uint8_t button);
static void release(uint8_t button);
static void assertNthEvent(ListOf(CtrlEvent*)* events, size_t i, CtrlActionType type, CtrlAction action, float value);
static void assertAction(CtrlEvent* event, CtrlActionType type, CtrlAction action, float value);
static void pressAndProcessButton(uint8_t button);
static void moveAndProcessAxis(uint8_t axis, float value);

void testNoEventHappening(void **state) {
    ctrlGlfwGamepadProcessIngameEvents(GLFW_JOYSTICK_1, &gamepad);

    events = ctrlInGameEventFetchAll();

    assert_int_equal(0, listSize(events));
}

void testPushingButtons(void **state) {
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_NORTH);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_EAST);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_SOUTH);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_WEST);

    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_DPAD_UP);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_DPAD_LEFT);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT);

    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER);

    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_LEFT_THUMB);
    pressAndProcessButton(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB);

    events = ctrlInGameEventFetchAll();

    assertNthEvent(events,  0, CTRL_MOV_JUMP, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  1, CTRL_MOV_RUN, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  2, CTRL_MOV_ACTIVATE, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  3, CTRL_MOV_CROUCH, CTRL_PRESS, 1.0f);

    assertNthEvent(events,  4, CTRL_FEAT_HEAD_LAMP, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  5, CTRL_FEAT_GOOGLES, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  6, CTRL_FEAT_CLEATS, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  7, CTRL_FEAT_GAS_MASK, CTRL_PRESS, 1.0f);

    assertNthEvent(events,  8, CTRL_WEAP_PREV, CTRL_PRESS, 1.0f);
    assertNthEvent(events,  9, CTRL_WEAP_NEXT, CTRL_PRESS, 1.0f);

    assertNthEvent(events, 10, CTRL_FEAT_MAP, CTRL_PRESS, 1.0f);
    assertNthEvent(events, 11, CTRL_FEAT_MAP_PDA, CTRL_PRESS, 1.0f);
}

void testPushingThenReleasingButton(void **state) {
    press(GLFW_GAMEPAD_BUTTON_SOUTH);
    ctrlGlfwGamepadProcessIngameEvents(GLFW_JOYSTICK_1, &gamepad);
    release(GLFW_GAMEPAD_BUTTON_SOUTH);
    ctrlGlfwGamepadProcessIngameEvents(GLFW_JOYSTICK_1, &gamepad);

    events = ctrlInGameEventFetchAll();

    assertNthEvent(events, 0, CTRL_MOV_ACTIVATE, CTRL_PRESS, 1.0f);
    assertNthEvent(events, 1, CTRL_MOV_ACTIVATE, CTRL_RELEASE, 0.0f);
}

void testMovingAllAxes(void **state) {
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, -0.25);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, -0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, -0.75);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, 0);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, 0.25);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, 0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_X, 0.75);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, -0.25);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, -0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, -0.75);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, 0.25);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, 0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_Y, 0.75);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, -0.75);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, 0.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_X, 0.75);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, -0.75);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, 0.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y, 0.75);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, -1.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, -0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 1.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, -1.0);

    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, -1.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, -0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.5);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 1.0);
    moveAndProcessAxis(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, -1.0);

    events = ctrlInGameEventFetchAll();

    uint32_t i = 0;

    assertNthEvent(events, i++, CTRL_MOV_STRAFE_LEFT, CTRL_PRESS, 0.25f);
    assertNthEvent(events, i++, CTRL_MOV_STRAFE_LEFT, CTRL_PRESS, 0.5f);
    assertNthEvent(events, i++, CTRL_MOV_STRAFE_LEFT, CTRL_PRESS, 0.75f);
    assertNthEvent(events, i++, CTRL_MOV_STRAFE_LEFT, CTRL_RELEASE, 0.0f);

    assertNthEvent(events, i++, CTRL_MOV_STRAFE_RIGHT, CTRL_PRESS, 0.25f);
    assertNthEvent(events, i++, CTRL_MOV_STRAFE_RIGHT, CTRL_PRESS, 0.5f);
    assertNthEvent(events, i++, CTRL_MOV_STRAFE_RIGHT, CTRL_PRESS, 0.75f);

    assertNthEvent(events, i++, CTRL_MOV_FORWARD, CTRL_PRESS, 0.25f);
    assertNthEvent(events, i++, CTRL_MOV_FORWARD, CTRL_PRESS, 0.5f);
    assertNthEvent(events, i++, CTRL_MOV_FORWARD, CTRL_PRESS, 0.75f);

    assertNthEvent(events, i++, CTRL_MOV_FORWARD, CTRL_RELEASE, 0.0f);
    assertNthEvent(events, i++, CTRL_MOV_BACKWARD, CTRL_PRESS, 0.25f);
    assertNthEvent(events, i++, CTRL_MOV_BACKWARD, CTRL_PRESS, 0.5f);
    assertNthEvent(events, i++, CTRL_MOV_BACKWARD, CTRL_PRESS, 0.75f);

    assertNthEvent(events, i++, CTRL_MOV_TURN_LEFT, CTRL_PRESS, 0.75f);
    assertNthEvent(events, i++, CTRL_MOV_TURN_LEFT, CTRL_RELEASE, 0.0f);
    assertNthEvent(events, i++, CTRL_MOV_TURN_RIGHT, CTRL_PRESS, 0.75f);

    assertNthEvent(events, i++, CTRL_MOV_LOOK_UP, CTRL_PRESS, 0.75f);
    assertNthEvent(events, i++, CTRL_MOV_LOOK_UP, CTRL_RELEASE, 0.0f);
    assertNthEvent(events, i++, CTRL_MOV_LOOK_DOWN, CTRL_PRESS, 0.75f);

    assertNthEvent(events, i++, CTRL_WEAP_SHOOT_SEC, CTRL_PRESS, 1.0f);
    assertNthEvent(events, i++, CTRL_WEAP_SHOOT_SEC, CTRL_RELEASE, 0.0f);

    assertNthEvent(events, i++, CTRL_WEAP_SHOOT_PRI, CTRL_PRESS, 1.0f);
    assertNthEvent(events, i++, CTRL_WEAP_SHOOT_PRI, CTRL_RELEASE, 0.0f);
}

static void moveAndProcessAxis(uint8_t axis, float value) {
    gamepad.axes[axis] = value;
    ctrlGlfwGamepadProcessIngameEvents(GLFW_JOYSTICK_1, &gamepad);
}

static void pressAndProcessButton(uint8_t button) {
    press(button);
    ctrlGlfwGamepadProcessIngameEvents(GLFW_JOYSTICK_1, &gamepad);
}

static void press(uint8_t button) {
    gamepad.buttons[button] = GLFW_PRESS;
}

static void release(uint8_t button) {
    gamepad.buttons[button] = GLFW_RELEASE;
}

static void assertNthEvent(ListOf(CtrlEvent*)* events, size_t i, CtrlActionType type, CtrlAction action, float value) {
    assert_true(i < listSize(events));
    CtrlEvent* event = optionalGet(listGet(events, i));
    assertAction(event, type, action, value);
}

static void assertAction(CtrlEvent* event, CtrlActionType type, CtrlAction action, float value) {
    logDebug("Expect: %s - %s (%g)", ctrlGetCtrlActionTypeName(type), ctrlGetCtrlActionName(action), value);
    logDebug("Actual: %s - %s (%g)", ctrlGetCtrlActionTypeName(event->type), ctrlGetCtrlActionName(event->action), event->value);
    assert_int_equal(type, event->type);
    assert_int_equal(action, event->action);
    assert_float_equal(value, event->value, 0.1f);
}

int main(int argc, char** argv){
    cmocka_set_message_output(CM_OUTPUT_TAP);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(testNoEventHappening, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testPushingButtons, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testPushingThenReleasingButton, setUp, tearDown),
        cmocka_unit_test_setup_teardown(testMovingAllAxes, setUp, tearDown),
    };

    int ret = cmocka_run_group_tests_name("odf/ctrl/glfw/gamepad.c", tests, NULL, NULL);

    return ret;
}
