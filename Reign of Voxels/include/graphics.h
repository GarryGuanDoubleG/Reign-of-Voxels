
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

extern sf::Window *g_window;

void GraphicsInit();
void draw(); // temporary

#endif