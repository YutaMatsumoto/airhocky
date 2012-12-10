

#ifndef INIT_SHADER_GUARD
#define INIT_SHADER_GUARD 

#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdio>
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

// Create a NULL-terminated string by reading the provided file
char*
readShaderSource(const char* shaderFile);

// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile);

#endif // INIT_SHADER_GUARD END
