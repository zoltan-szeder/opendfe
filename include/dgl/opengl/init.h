#ifndef DGL_OPENGL_INIT_H
#define DGL_OPENGL_INIT_H

#include <GL/gl.h>

unsigned int initializeOpenGl();

unsigned int loadShaderProgram(char* vsFile, char* fragFile);

#endif
