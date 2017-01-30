#include "game.h"
#include "client.h"
#include "shader.h"
//doing this just to test menu out. should refactor soon
//sf::Font main_font; 

//load fonts.
//should probably make a font utility class
//and load everything from start up using cjson for file locations
void LoadFonts()
{
}

void Initialize()
{
	//initialize funcitons
	GraphicsInit();
	compile_shaders();
}


void MenuLoop()
{
	static sf::Font main_font;
	std::string input = "";

	if (!main_font.loadFromFile("Resources/fonts/Quicksand-Regular.otf"))
		printf("Could not load font %s\n", "Resources/fonts/Quicksand-Regular.otf");

	sf::Text text("", main_font, 50);
	text.setFont(main_font);
	text.setCharacterSize(100);
	text.setFillColor(sf::Color(255,0,0,255));
	text.setOutlineColor(sf::Color::Red);
	text.setOutlineThickness(1.75f);
	text.setStyle(sf::Text::Bold);

	while (g_window->isOpen())
	{
		sf::Event event;
		while (g_window->pollEvent(event))
		{
			
		}
		text.setString(input);
	}
}

void GameLoop()
{
	//go to menu then start game
	//later implement a scene manager that runs the scene's loop
	//instead of calling it explicitly
	MenuLoop();

}