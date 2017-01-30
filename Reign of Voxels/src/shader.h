#pragma once
#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_
#include "GL/glew.h"

//#include <GL/freeglut.h>//include this last

extern GLuint g_shader_prog;
GLuint compile_shaders(void);

#endif // ! _SHADER_H_