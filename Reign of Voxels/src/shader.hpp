#pragma once
#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_
#include "GL/glew.h"
extern GLuint g_shader_prog; /**<global basic mvp shader and texture drawing*/  
/**
* @brief compiles a vertex and fragment shader and attaches it into a program
* @return id of compiled program
*/
GLuint compile_shaders(void);
#endif // ! _SHADER_H_