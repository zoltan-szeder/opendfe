#ifndef ODF_OGL_INIT_H
#define ODF_OGL_INIT_H

#include <stdint.h>

#include <GL/gl.h>

uint32_t initializeOpenGl();

uint32_t loadShaderProgram(char* vsFile, char* fragFile);

#endif
