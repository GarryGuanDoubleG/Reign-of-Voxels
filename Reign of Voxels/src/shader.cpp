#include <stdio.h>
#include <stdlib.h>
#include "shader.h"

#define MAX_STR_LEN 4096

GLuint g_shader_prog;

//checks for errors while compiling shader and prints it out
//and returns GL_TRUE if error was found
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

//compiles shader and attaches it to program
//returns false if error occurs
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
