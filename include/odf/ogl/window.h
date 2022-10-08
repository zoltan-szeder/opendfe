#ifndef ODF_OGL_WINDOW
#define ODF_OGL_WINDOW

#include <GLFW/glfw3.h>
#include <stdbool.h>

GLFWwindow* initializeWindow();
bool windowShouldTerminate(GLFWwindow*);
void terminateWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

#endif
