#include "game.h"
#include "Gui\Menu.h"
#include "client.h"
#include "shader.h"
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

	while (g_window->isOpen())
	{
		draw();
	}
}