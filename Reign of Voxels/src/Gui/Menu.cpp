#include "Menu.h"
#include "scene.h"
#include "graphics.h"
#include "label.h"
#include "button.h"`
#include "simple_logger.h"

Menu::Menu()
{	
	m_layout = new Layout();
	m_widgets = m_layout->LoadMenuLayout(LoginMenu);
	//set parent for callback
	//seems like some coupling but good enough?
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->setParent(this);
	}
}

//handle menu loop
//will later extend class for different types of menus
//doing login first to set up for network
void Menu::SceneLoop()
{

	sf::Event event;
	while (g_window->pollEvent(event))
	{
		HandleInput(event);
	}	
	Render();
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
}

void Menu::triggerCallBack(sf::String event)
{
	Json data;
	if (event == "login")
	{
		for (int i = 0; i < m_widgets.size(); i++)
		{
			// get textbox data and merge it into a single obj
			Json widget_data = m_widgets[i]->getData();
			
			if (widget_data.is_null())
				continue;

			for (Json::iterator it = widget_data.begin(); it != widget_data.end(); ++it)
			{
				data[it.key()] = it.value();
			}
		}
		//broadcast login event
		Game::instance().getEventSystem().Notify(Event::Login, data);
	}

}