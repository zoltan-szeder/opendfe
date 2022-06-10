#include <GL/glew.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "dgl/main.h"
#include "dgl/model.h"
#include "dgl/opengl/init.h"
#include "dgl/opengl/window.h"

struct Display {
    GLFWwindow* window;
    unsigned int guiShader;
};

Display* dglCreateDisplay() {
    GLFWwindow* window = initializeWindow();
    if (window == NULL) return NULL;

    unsigned int shaderProgram = initializeOpenGl();
    if (shaderProgram == 0) return NULL;

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    Display* display = malloc(sizeof(Display));
    display->window = window;
    display->guiShader = shaderProgram;
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

        glUseProgram(display->guiShader);

        dglModelDraw(state->model);

        glfwSwapBuffers(display->window);
        glfwPollEvents();
}

void dglDestroyDisplay(Display* display){
    glDeleteProgram(display->guiShader);
    terminateWindow();
    free(display);
}
