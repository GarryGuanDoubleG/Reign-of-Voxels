#include "game.h"
#include "Gui\Menu.h"
#include "client.h"
#include "shader.h"
#include "graphics.h"

//doing this just to test menu out. should refactor soon
//sf::Font main_font; 

//load fonts.
//should probably make a font utility class
//and load everything from start up using cjson for file locations

float g_time;//global time
float g_time_prev;
float g_delta_time; //delta time

void LoadFonts()
{
}

void Initialize()
{
	//initialize funcitons
	GraphicsInit();
	compile_shaders();
}


void GameLoop()
{
	//Menu *menu = new Menu();
	//menu->MenuLoop();//test it first
	sf::Vector2u win_size = g_window->getSize();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)win_size.x / win_size.y, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 mvp = projection * view * model;

	sf::Event event;
	while (g_window->isOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw(mvp);

		while (g_window->pollEvent(event))
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				g_window->close();
				break;
			}
		}
	}
}