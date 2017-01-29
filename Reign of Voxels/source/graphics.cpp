#include "graphics.h"
#include "shader.h"

sf::RenderWindow *g_window;

static const GLfloat points[] = { -0.1, 0, 0,
.1, 0, 0,
0, .2, 0 };
GLuint vbo;

void draw()
{
	glUseProgram(g_shader_prog);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GraphicsInit()
{
	int err;
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 4;
	settings.minorVersion = 5;
	//Create context
	g_window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Reign of Voxels", sf::Style::Default, settings);
	g_window->setFramerateLimit(60);

	if ((err = glewInit()) != GLEW_OK)
	{
		printf("Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderObject()
{

}