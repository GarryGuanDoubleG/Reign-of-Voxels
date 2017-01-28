#include "game.h"
#include "client.h"

Font *fonts;

void LoadFonts()
{
	sf::Font font;
	
	if (!font.loadFromFile("Resources/fonts/Quicksand-Regular.otf"))
		printf("Could not load font %s\n", "Resources/fonts/Quicksand-Regular.otf");
}

void Initialize()
{
	//initialize funcitons
	GraphicsInit();
	LoadFonts();
}

void MenuLoop()
{
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

		}
		// clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		g_window->display();
	}
}

void GameLoop()
{
	//go to menu then start game
	MenuLoop();

	while (g_window->isOpen())
	{
		sf::Event event;
		while (g_window->pollEvent(event))
		{
			if ((event.type == sf::Event::Closed) || 
				(event.key.code == sf::Keyboard::Escape))
				g_window->close();
		}
		// clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw(); 
		g_window->display();
	}

}