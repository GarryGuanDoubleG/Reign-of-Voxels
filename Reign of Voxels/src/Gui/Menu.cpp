#include "Menu.h"
#include "graphics.h"


Menu::Menu()
{
	if (!font_.loadFromFile("Resources/fonts/Quicksand-Regular.otf"))
		printf("Could not load font %s\n", "Resources/fonts/Quicksand-Regular.otf");

	state_ = STATE_IDLE;

	//setting fonts
	text_.setFillColor(sf::Color::Red);
	text_.setCharacterSize(50);
	text_.setFont(font_);
	text_.setOutlineThickness(1.75f);
}

//handle menu loop
//will later extend class for different types of menus
//doing login first to set up for network
void Menu::MenuLoop()
{
	while (g_window->isOpen && state_ == STATE_LOGIN)
	{
		sf::Event event;
		while (g_window->pollEvent(event))
		{
			HandleInput(event);
		}	

		Render();
	}
}

//render the menu to the screen
//will cover the entire window
void Menu::Render()
{
	g_window->clear(sf::Color::Black);
	//save gl states
	g_window->pushGLStates();
	//draw text
	g_window->draw(text_);
	//restore the gl states
	g_window->popGLStates();
	
	g_window->display();
}

void Menu::HandleInput(sf::Event event)
{
	if ((event.type == sf::Event::Closed))
		g_window->close();
	switch (state_)
	{
	case STATE_IDLE:
		break;
	case STATE_TEXTBOX:
		break;
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