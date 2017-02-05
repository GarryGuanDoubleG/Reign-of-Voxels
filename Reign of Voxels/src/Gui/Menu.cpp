#include "Menu.h"
#include "graphics.h"
#include "label.h"
#include "button.h"

Menu::Menu()
{
	state_ = STATE_IDLE;
	
	m_layout = new Layout();
	m_widgets = m_layout->LoadMenuLayout(Login);
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
	//set background to black
	g_window->clear(sf::Color::Black);
	//save gl states
	g_window->pushGLStates();
	//draw text
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->draw(*g_window, sf::RenderStates::Default);
	}
	//restore the gl states
	g_window->popGLStates();
	g_window->display();
}

void Menu::HandleInput(sf::Event event)
{
	if ((event.type == sf::Event::Closed) || event.key.code == sf::Keyboard::Escape)
		g_window->close();

	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->HandleInput(event);
	}

	g_window->display();
}