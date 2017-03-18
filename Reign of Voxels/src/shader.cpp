#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "simple_logger.h"
#include "shader.hpp"
#include "json.hpp"
#define MAX_STR_LEN 4096

static std::map<std::string, GLuint> g_shader_map;

using Json = nlohmann::json;
/**
*@brief prints out any errors with compiling a particular shader
*@param name the name of the shader
*@param shader the compiled shader to check
*/
GLuint check_shader_err(char *name, GLuint shader)
{
	GLchar infoLog[512];

	GLint result = GL_FALSE;
	GLint InfoLogLength;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
		glGetShaderInfoLog(shader, InfoLogLength, &InfoLogLength, infoLog);

		slog("Shader Error %s: %s\n", name, infoLog);

		return GL_FALSE;
	}
	return GL_TRUE;
}

bool read_file(char *name, std::string &out)
{
	std::ifstream shader_if(name, std::ifstream::in);
	std::string input;

	if (!shader_if.is_open())
	{
		slog("Read_File: Failed to open file %s", name);
		return false;
	}

	while (std::getline(shader_if, input))
	{
		out.append(input + "\n");
	}

	return true;
}
/**
* @brief takes the filename of a shader, compiles it and attaches it to a given program
* @param filename the path and filename of the shader source
* @param type the type of the shader
* @param program the program to attach the shader to
* @return GL_TRUE if shader compiles without error, false if not
*/
bool compile_attach_shader(std::string filename, GLuint type, GLuint program)
{
	GLuint shader;

	std::string shader_str;
	const char * shader_src;

	if (!read_file((char *)filename.c_str(), shader_str)) return false;

	shader_src = shader_str.c_str();

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_src, nullptr);
	glCompileShader(shader);

	//check if compiled successfully
	if (check_shader_err((char *)filename.c_str(), shader))
	{
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
	else
	{
		return false;//error
	}
}

/**
* @brief compiles a vertex and fragment shader and attaches it into a program
* @return id of compiled program
*/
GLuint compile_shaders(std::string &vs_shader, std::string &fs_shader)
{
	GLuint program;
	GLchar infoLog[512];
	GLint result = GL_FALSE;
	GLint InfoLogLength;

	//Create program, attach shaders to it, and link it
	program = glCreateProgram();

	compile_attach_shader(vs_shader, GL_VERTEX_SHADER, program);
	compile_attach_shader(fs_shader, GL_FRAGMENT_SHADER, program);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		glGetProgramInfoLog(program, InfoLogLength, &InfoLogLength, infoLog);

		printf("Program Log: %s\n", infoLog);
	}
	return program;
}


void DeleteAllShaders()
{
	for (auto &program : g_shader_map)
	{
		glDeleteProgram(program.second);
	}
	g_shader_map.clear();
}

void CompileAllShaders()
{
	std::ifstream in("shader/shader.json");
	if (!in.is_open())
	{
		std::cout << "Shader json not found \n";
	}
	Json shaders;
	in >> shaders;

	for (Json::iterator it = shaders.begin(); it != shaders.end(); ++it)
	{
		Json obj = it.value();
		std::string key = it.key();

		std::string vs_filename = obj["vs"];
		std::string fs_filename = obj["fs"];

		g_shader_map.insert(std::pair<std::string, GLuint>(key, compile_shaders(vs_filename, fs_filename)));
	}

	atexit(DeleteAllShaders);
}

GLuint GetShader(std::string name)
{
	if (g_shader_map.find(name) == g_shader_map.end())
	{
		slog("Could not find Shader: %s", name.c_str());
	}

	return g_shader_map[name];
}