#ifndef GL_INIT_H
#define GL_INIT_H

#include <GL/gl.h>

unsigned int initializeOpenGl();

unsigned int loadShaderProgram(char* vsFile, char* fragFile);

#endif
