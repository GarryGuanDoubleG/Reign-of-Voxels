#include "game.h"
#include "Gui\Menu.h"
#include "client.h"
#include "shader.h"
#include "graphics.h"
#include "camera.h"
//doing this just to test menu out. should refactor soon
//sf::Font main_font; 

//load fonts.
//should probably make a font utility class
//and load everything from start up using cjson for file locations

sf::Clock g_delta_clock;
sf::Clock g_clock;

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
	Camera *camera = new Camera();
	glm::mat4 model = glm::mat4(1.0f);

	while (g_window->isOpen())
	{
		sf::Event event;

		g_delta_clock.restart();

		Mat4 mvp = camera->GetProj() * camera->GetCamView() * model;
		draw(mvp);

		while (g_window->pollEvent(event))
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				GraphicsClose();
				break;
			}
			camera->HandleInput(event);
		}
	}
}