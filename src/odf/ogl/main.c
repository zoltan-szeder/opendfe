#include <GL/glew.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "odf/ogl/main.h"
#include "odf/ogl/model.h"
#include "odf/ogl/init.h"
#include "odf/ogl/window.h"

#include "odf/sys/memory.h"

struct Display {
    GLFWwindow* window;
    unsigned int hudShader;
};

Display* dglCreateDisplay() {
    GLFWwindow* window = initializeWindow();
    if (window == NULL) return NULL;

    unsigned int shaderProgram = initializeOpenGl();
    if (shaderProgram == 0) return NULL;


    Display* display = memoryAllocateWithTag(sizeof(Display), "odf/ogl/main/Display");
    display->window = window;
    display->hudShader = shaderProgram;
    return display;
}

GameControl* dglReadInputs(Display* display, GameControl* gameControl) {

    if(glfwGetKey(display->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(display->window, true);
        gameControl->shouldExit = true;
    }

    return gameControl;
}

void dglDraw(Display* display, GameState* state) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(display->hudShader);
        glUniform1i(glGetUniformLocation(display->hudShader, "texture1"), 0);

        dglModelDraw(state->model);

        glfwSwapBuffers(display->window);
        glfwPollEvents();
}

void dglDestroyDisplay(Display* display){
    glDeleteProgram(display->hudShader);
    terminateWindow();
    memoryRelease(display);
}
