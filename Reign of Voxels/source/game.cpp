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
			if ((event.type == sf::Event::Closed) ||
				(event.key.code == sf::Keyboard::Escape))
			{
				g_window->close();
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode >= 32 && event.text.unicode < 126)
				{
					printf("Text event\n");
					input += (char)event.text.unicode;
				}
				else if (event.text.unicode == 8 && input.length() > 0)
				{
					input.erase(input.length() - 1, input.size());
				}
			}
		}
		text.setString(input);
		g_window->clear(sf::Color::Blue);
		g_window->pushGLStates();
		g_window->draw(text);
		g_window->popGLStates();
		draw();		
		g_window->display();
	}
}

void GameLoop()
{
	//go to menu then start game
	MenuLoop();

	//while (g_window->isOpen())
	//{
	//	sf::Event event;
	//	while (g_window->pollEvent(event))
	//	{
	//		if ((event.type == sf::Event::Closed) ||
	//			(event.key.code == sf::Keyboard::Escape))
	//		{
	//			g_window->close();
	//		}
	//	}
	//}
}