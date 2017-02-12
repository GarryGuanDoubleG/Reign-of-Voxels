#include "Menu.h"
#include "scene.h"
#include "layout.h"
#include "graphics.h"
#include "label.h"
#include "button.h"`
#include "simple_logger.h"

Menu::Menu(MenuLayouts layout) : m_layout(new Layout())
{
	m_widgets = m_layout->LoadMenuLayout(layout);
	//set parent for callback
	//seems like some coupling but good enough?
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->setParent(this);
	}
};

Menu::Menu(MenuLayouts layout, Json &data)
	: m_layout(new Layout())
{
	m_widgets = m_layout->LoadMenuLayout(layout);

	UpdateWidgets(data);
	for (int i = 0; i < m_widgets.size(); i++)
		m_widgets[i]->setParent(this);

	Game::instance().getEventSystem().addObserver(this);
}

Menu::~Menu()
{
	Game::instance().getEventSystem().removeObserver(this);
	for (int i = 0; i < m_widgets.size(); i++)
	{
		delete(m_widgets[i]);
		m_widgets[i] = NULL;
	}

	delete(m_layout);
}

void Menu::UpdateWidgets(Json &data)
{
	std::cout << "update labels " << data.dump() << std::endl;

	for (int i = 0; i < m_widgets.size(); i++)
	{
		std::string id = m_widgets[i]->getID();	
		if (data.find(id) != data.end())		
			m_widgets[i]->setString(data[id]);
		else if (id == "p1" || id == "p2" || id == "p3" || id == "p4")
		{
			m_widgets[i]->setString(""); //must have left
		}
	}
}

void Menu::onNotify(Event event, Json &data)
{
	switch (event)
	{
	case JoinPlayer:
		UpdateWidgets(data);
		break;
	case LeavePlayer:
		UpdateWidgets(data);
		break;	
	}
}

//handle menu loop
//will later extend class for different types of menus
//doing login first to set up for network
void Menu::SceneFrame()
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
	if (event.key.code == sf::Keyboard::Escape)
	{
		Json data = nullptr;
		Game::instance().getEventSystem().Notify(Close, data);
		GraphicsClose();
	}
	for (int i = 0; i < m_widgets.size(); i++)
		m_widgets[i]->HandleInput(event);
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
				if (it.key() == "port")
				{
					std::string port = it.value(); // store it as an int
					data["port"] = std::stoi(port);
				}
				else
					data[it.key()] = it.value();
			}
		}
		//broadcast login event
		Game::instance().getEventSystem().Notify(Event::Login, data);
	}

	if (event == "start")
	{
		Json data;
		data["event"] = Event::Start;
		Game::instance().getEventSystem().Notify(Event::Start, data);
	}

}