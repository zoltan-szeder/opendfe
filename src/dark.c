#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl/init.h"

#include "glmipmap.h"
#include "gltexture.h"
#include "glwindow.h"
#include "gl/init.h"
#include "file.h"
#include "gob.h"
#include "bm.h"


int main(int argc, char** argv) {
    GLFWwindow* window = initializeWindow();
    if (window == NULL) return -1;

    unsigned int shaderProgram = initializeOpenGl();
    if (shaderProgram == 0) return 1;

    if(argc < 3) return 1;
    GobArchive* archive = gobOpenArchive(argv[1]);
    GobFile* file = gobGetFile(archive, argv[2]);
    InMemoryFile* inMem = gobReadFile(file);
    BMFile* bmFile = bmOpenInMemoryFile(inMem);

    uint32 texture = bmGlBindImageTexture(bmFile, NULL);

    bmCloseFile(bmFile);
    gobCloseFile(inMem);
    gobCloseArchive(archive);



    while(!windowShouldTerminate(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    terminateWindow();

    return 0;
}

