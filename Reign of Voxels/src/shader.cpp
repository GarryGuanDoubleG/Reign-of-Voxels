#include <stdio.h>
#include <stdlib.h>
#include "shader.hpp"

#define MAX_STR_LEN 4096

GLuint g_shader_prog;

/**
*@brief prints out any errors with compiling a particular shader
*@param shader the compiled shader to check
*/
GLuint check_shader_err(GLuint shader)
{
	GLchar infoLog[512];

	GLint result = GL_FALSE;
	GLint InfoLogLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		glGetShaderInfoLog(shader, InfoLogLength, &InfoLogLength, infoLog);

		printf("Shader Compile Log: %s\n", infoLog);

		return GL_FALSE;
	}

	return GL_TRUE;
}

/**
* @brief takes the filename of a shader, compiles it and attaches it to a given program
* @param filename the path and filename of the shader source
* @param type the type of the shader
* @param program the program to attach the shader to
* @return GL_TRUE if shader compiles without error, false if not
*/
int compile_attach_shader(char *filename, GLuint type, GLuint program)
{
	GLuint shader;

	char in;
	char shader_cbuffer[MAX_STR_LEN];
	const char *shader_src;
	int i = 0;
	FILE *fp;

	fp = fopen(filename, "r");
	if (!fp)
	{
		printf("failed to open shader %s", filename);
		return -1;
	}

	while (fscanf(fp, "%c", &in) > 0)
	{
		shader_cbuffer[i++] = in;
	}
	shader_cbuffer[i] = '\0';
	shader_src = shader_cbuffer;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);


	//check if compiled successfully
	if (check_shader_err(shader))
	{
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
	else
	{
		return GL_FALSE;//error
	}

	return GL_TRUE;
}
/**
* @brief compiles a vertex and fragment shader and attaches it into a program
* @return id of compiled program
*/
GLuint compile_shaders(void)
{
	GLuint program;
	GLchar infoLog[512];
	GLint result = GL_FALSE;
	GLint InfoLogLength;

	//Create program, attach shaders to it, and link it
	program = glCreateProgram();

	compile_attach_shader("shader/vs_shader.glsl", GL_VERTEX_SHADER, program);
	compile_attach_shader("shader/fs_shader.glsl", GL_FRAGMENT_SHADER, program);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		glGetProgramInfoLog(program, InfoLogLength, &InfoLogLength, infoLog);

		printf("Program Log: %s\n", infoLog);
	}

	g_shader_prog = program;

	return program;
}
