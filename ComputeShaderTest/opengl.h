#ifndef _OPENGL_H
#define _OPENGL_H

#define GL_GLEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES

#include <GL\glew.h>
#include <GL/gl.h>

// This includes the new stuff, supplied by the application
// #include "GL/glext.h"

#include <string>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void initGL();
void swapBuffers();

// Return handles
GLuint genTexture(GLuint& tex_handle);
GLuint genRenderProg(GLuint tex_handle, GLuint& vao_handle); // Texture as the param
GLuint genComputeProg(GLuint);

void checkErrors(std::string);

#endif
