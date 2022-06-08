#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "gl/init.h"
#include "file.h"

unsigned int loadShader(GLenum shaderType, char* glslFile);

unsigned int initializeOpenGl() {
	GLenum status = glewInit();

	if(status != GLEW_OK) {
		fprintf(stderr, "Could not initialize GLEW: %s\n", glewGetErrorString(status));
        return status;
	}

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	return loadShaderProgram("src/shaders/gui.vs", "src/shaders/gui.frag");
}

unsigned int loadShaderProgram(char* vsFile, char* fragFile) {
	unsigned int shaderProgram = glCreateProgram();

    unsigned int vertexShader = loadShader(GL_VERTEX_SHADER, vsFile);
    if(vertexShader == 0) return 0;
	glAttachShader(shaderProgram, vertexShader);

    unsigned int fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragFile);
    if(fragmentShader == 0) return 0;
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

    int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
        char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int loadShader(GLenum shaderType, char* glslFile) {
	char* glsl = fileReadAll(glslFile);

	unsigned int shader = glCreateShader(shaderType);
    if(shader == 0) return 0;

	glShaderSource(shader, 1, (const char**) &glsl, NULL);
	glCompileShader(shader);

	free(glsl);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "ERROR::SHADER::%d::COMPILATION_FAILED\n%s",shaderType, infoLog);
        return 0;
	}

	return shader;
}
