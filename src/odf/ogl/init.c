#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "odf/ogl/init.h"
#include "odf/sys/file.h"
#include "odf/sys/optional.h"
#include "odf/sys/memory.h"

OptionalPtr* loadShader(unsigned int* shaderId, GLenum shaderType, char* glslFile);

unsigned int initializeOpenGl() {
	GLenum status = glewInit();

	if(status != GLEW_OK) {
		fprintf(stderr, "Could not initialize GLEW: %s\n", glewGetErrorString(status));
        return status;
	}

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	return loadShaderProgram("src/odf/ogl/shaders/hud.vs", "src/odf/ogl/shaders/hud.frag");
}

unsigned int loadShaderProgram(char* vsFile, char* fragFile) {
	unsigned int shaderProgram = glCreateProgram();

    unsigned int vertexShader;
    OptionalPtr* optionalVertexShader = loadShader(&vertexShader, GL_VERTEX_SHADER, vsFile);
    if(optionalIsEmpty(optionalVertexShader)) {
		optionalPrint(stderr, (void*) optionalVertexShader);
		return 0;
	};
	glAttachShader(shaderProgram, vertexShader);

    unsigned int fragmentShader;
	OptionalPtr* optionalFragmentShader = loadShader(&fragmentShader,GL_FRAGMENT_SHADER, fragFile);
    if(optionalIsEmpty(optionalFragmentShader)) {
		optionalPrint(stderr, (void*) optionalFragmentShader);
		return 0;
	}
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

OptionalPtr* loadShader(unsigned int* shaderId, GLenum shaderType, char* glslFile) {
	OptionalPtr* optionalGlslFile = fileReadAll(glslFile);
	if(optionalIsEmpty(optionalGlslFile)) {
		optionalPrint(stderr, (void*) optionalGlslFile);
		return optionalGlslFile;
	}

	char* glsl = optionalGet(optionalGlslFile);

	*shaderId = glCreateShader(shaderType);
    if(*shaderId == 0)
		return optionalEmpty("loadShader - Could not allocate shader memory");

	glShaderSource(*shaderId, 1, (const char**) &glsl, NULL);
	glCompileShader(*shaderId);

	memoryRelease(glsl);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shaderId, 512, NULL, infoLog);
		fprintf(stderr, "ERROR::SHADER::%d::COMPILATION_FAILED\n%s",shaderType, infoLog);
        return optionalEmpty("loadShader - Could not compile shader");
	}

	return optionalOf(shaderId);
}
