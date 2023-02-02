#ifndef ODF_CTRL_GLFW_GAMEPAD_H
#define ODF_CTRL_GLFW_GAMEPAD_H

#include <GLFW/glfw3.h>
#include <stdint.h>

// Add the Linux input event definitions for right side gamepad buttons
#define GLFW_GAMEPAD_BUTTON_NORTH GLFW_GAMEPAD_BUTTON_TRIANGLE
#define GLFW_GAMEPAD_BUTTON_SOUTH GLFW_GAMEPAD_BUTTON_CROSS
#define GLFW_GAMEPAD_BUTTON_EAST GLFW_GAMEPAD_BUTTON_CIRCLE
#define GLFW_GAMEPAD_BUTTON_WEST GLFW_GAMEPAD_BUTTON_SQUARE

void ctrlGlfwGamepadInit();
void ctrlGlfwGamepadProcessAllIngameEvents();
void ctrlGlfwGamepadProcessIngameEvents(uint8_t gamepadNum, const GLFWgamepadstate* const state);

#endif
