#include "game.h"

void Initialize()
{
	GraphicsInit();
}

void GameLoop()
{
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