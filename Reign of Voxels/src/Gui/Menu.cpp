#include "Menu.h"
#include "graphics.h"


Menu::Menu()
{
	m_font = new sf::Font();
	if (!m_font->loadFromFile("Resources/fonts/Quicksand-Regular.otf"))
		printf("Could not load font %s\n", "Resources/fonts/Quicksand-Regular.otf");

	state_ = STATE_IDLE;
	box = new TextBox(m_font, 100.0f);

	//setting fonts
	text_.setFillColor(sf::Color::Red);
	text_.setCharacterSize(50);
	text_.setFont(*m_font);
	text_.setOutlineThickness(1.75f);
}

//handle menu loop
//will later extend class for different types of menus
//doing login first to set up for network
void Menu::MenuLoop()
{
	while (g_window->isOpen())
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
		//no gui componenets are selected
	case STATE_IDLE:
		break;
	case STATE_TEXTBOX:
		break;
	}
}