#include "simple_logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

FILE * __log_file = NULL;

void close_logger()
{
	if (__log_file != NULL)
	{
		fclose(__log_file);
		__log_file = NULL;
	}
}

void init_logger(const char *log_file_path)
{
	if (log_file_path == NULL)
	{
		__log_file = fopen("output.log", "a");
	}
	else
	{
		__log_file = fopen(log_file_path, "a");
	}
	atexit(close_logger);
}


void _slog(char *f, int l, char *msg, ...)
{
	va_list ap;
	/*echo all logging to stdout*/
	va_start(ap, msg);

	char buffer[256];
	strcpy(buffer, f);
	char * tok;

	tok = strtok(buffer, "\\");
	while (tok)
	{
		char * temp = strtok(NULL, "\\");
		if (!temp)
			break;
		tok = temp;
	}

	fprintf(stdout, "%s:%i: ", tok, l);
	vfprintf(stdout, msg, ap);
	fprintf(stdout, "\n");
	va_end(ap);
	if (__log_file != NULL)
	{
		va_start(ap, msg);
		fprintf(__log_file, "%s:%i: ", tok, l);
		vfprintf(__log_file, msg, ap);
		fprintf(__log_file, "\n");
		va_end(ap);
	}
}


//call back function in case glfw spots an error
void ErrorCallback(int error, const char* description)
{
	printf("Error: %s\n", description);
}

const char * GetGLErrorStr(GLenum err)
{
	switch (err)
	{
	case GL_NO_ERROR:          return "No error";
	case GL_INVALID_ENUM:      return "Invalid enum";
	case GL_INVALID_VALUE:     return "Invalid value";
	case GL_INVALID_OPERATION: return "Invalid operation";
	case GL_STACK_OVERFLOW:    return "Stack overflow";
	case GL_STACK_UNDERFLOW:   return "Stack underflow";
	case GL_OUT_OF_MEMORY:     return "Out of memory";
	default:                   return "Unknown error";
	}
}

void CheckGLError()
{
	static int err_count = 0;
	while (GL_TRUE)
	{
		const GLenum err = glGetError();
		if (GL_NO_ERROR == err)
			break;
		printf("GL ERROR %i: %s\n", ++err_count, GetGLErrorStr(err));
	}
}

void _SlogCheckGLError(char *f, int l)
{
	static int err_count = 0;
	while (GL_TRUE)
	{
		char buffer[128];
		const GLenum err = glGetError();
		if (GL_NO_ERROR == err)
			break;
		sprintf(buffer, "GL ERROR %i: %s\n", ++err_count, GetGLErrorStr(err));
		_slog(f, l, buffer);
	}
}