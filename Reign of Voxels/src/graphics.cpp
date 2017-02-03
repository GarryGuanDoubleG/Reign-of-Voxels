#include <glm/gtc/type_ptr.hpp>
#include "graphics.h"
#include "shader.h"
#include "simple_logger.h"
#include "texture.hpp"

sf::RenderWindow *g_window;

void GraphicsInit()
{
	int err;
	sf::ContextSettings settings;
	GLuint vert_buff, uv_buff;

	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = .0;
	//Create context
	g_window = new sf::RenderWindow(sf::VideoMode(1280, 720), "Reign of Voxels", sf::Style::Default, settings);
	g_window->setMouseCursorGrabbed(true);
	g_window->setMouseCursorVisible(false);
	g_window->setFramerateLimit(60);

	if ((err = glewInit()) != GLEW_OK)
	{
		printf("Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
}

//will need a resource manager to handle 
//cleaning up the gpu states
void GraphicsClose()
{
	g_window->close();
}

sf::Vector2f GraphicsGetWinSizef()
{
	float w = g_window->getSize().x;
	float h = g_window->getSize().y;

	return sf::Vector2f(w, h);
}

sf::Vector2u GraphicsGetWinSizeu()
{
	return g_window->getSize();
}

