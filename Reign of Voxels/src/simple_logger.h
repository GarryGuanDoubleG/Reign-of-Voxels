#ifndef __SIMPLE_LOGGER__
#define __SIMPLE_LOGGER__
/**
* simple_logger
* @license The MIT License (MIT)
*   @copyright Copyright (c) 2015 EngineerOfLies
*    Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, EventSystem to the following conditions:
*    The above copyright notice and this permission notice shall be included in all
*    copies or substantial portions of the Software.
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*    SOFTWARE.
*/
#include <GL/glew.h>


/**
@brief initializes the simple logger.  Will automatically cleanup at program exit.
@param log_file_path the file to log to
*/
void init_logger(const char *log_file_path);

/**
@brief logs a message to stdout and to the configured log file
@param msg a string with tokens
@param ... variables to be put into the tokens.
*/
#define slog(...) _slog(__FILE__,__LINE__,__VA_ARGS__)
void _slog(char *f, int l, char *msg, ...);

/*
*	author Garry Guan
*/

//call back function in case glfw spots an error
void ErrorCallback(int error, const char* description);
const char * GetGLErrorStr(GLenum err);
void CheckGLError();
/*
* end/Garry Guan
*/
#define SlogCheckGLError(...) _SlogCheckGLError(__FILE__,__LINE__)
void _SlogCheckGLError(char *f, int l);

#endif