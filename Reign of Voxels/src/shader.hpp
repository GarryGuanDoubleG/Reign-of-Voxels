#pragma once
#pragma once
#ifndef _SHADER_H_
#define _SHADER_H_
#include "GL/glew.h"
/**
* @brief compiles a vertex and fragment shader and attaches it into a program
* @return id of compiled program
*/
void CompileAllShaders();

GLuint GetShader(std::string name);
#endif // ! _SHADER_H_