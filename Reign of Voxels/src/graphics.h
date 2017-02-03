
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "glm.h"


extern sf::RenderWindow *g_window;

void GraphicsInit();
void GraphicsClose();

sf::Vector2f GraphicsGetWinSizef();
sf::Vector2u GraphicsGetWinSizeu();



#endif