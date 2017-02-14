#include "Menu.h"
#include "scene.h"
#include "layout.h"
#include "game.h"
#include "label.h"
#include "button.h"`
#include "simple_logger.h"
/**
* A constructor.
* Specifies to the layout class which type of menu -login or lobby - to load
* and sets the parent of generated widgets to this menu
* @param layout the menu layout to load
*/
Menu::Menu(MenuLayouts layout) : m_layout(new Layout())
{
	m_widgets = m_layout->LoadMenuLayout(layout);
	//set parent for callback
	//seems like some coupling but good enough?
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->setParent(this);
	}
	Game::instance().getEventSystem().addObserver(this);
};
/**
* A constructor.
* Overloaded constructor. Specifies to the layout class which type of menu -login or lobby - to load
* and updates widget information from given json data. Example is names of players joined in lobby
* @param layout the menu layout to load
* @param data json container with widget IDs as the key of the data to update the widgets with
*/
Menu::Menu(MenuLayouts layout, Json &data)
	: m_layout(new Layout())
{
	m_widgets = m_layout->LoadMenuLayout(layout);

	UpdateWidgets(data);
	for (int i = 0; i < m_widgets.size(); i++)
		m_widgets[i]->setParent(this);

	Game::instance().getEventSystem().addObserver(this);
}
/**
* A destructor.
* frees the widgets and unsubscribes from event system
*/
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
/**.
* @brief Loops through each widget and updates string if widget id matches key in passed in data
* @param data the data with new widget information with keys the same as the widget id.
*/
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
/**.
* @brief callback function from global event system. Updates widget information (new player names from joining lobby)
* @param event the event type used to handle data passed
* @param data the json container which information to update widgets with (join / leaving player names)
*/
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
/**.
* @brief callback function from user input event. forwards the input to HandleInput method
* @param event tells us it's from server or client
* @param input the user input
*/
void Menu::onNotify(Event event, sf::Event &input)
{
	HandleInput(input);
}

/**.
* @brief Renders menu every scene.
*/
void Menu::SceneFrame()
{
	Render();
}

/**
* @brief Clears window background to black and tells widgets to draw themselves to window
*/
void Menu::Render()
{
	//set background to black
	Game::instance().getWindow()->clear(sf::Color::Black);
	//save gl states
	Game::instance().getWindow()->pushGLStates();
	//draw text
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->draw(*Game::instance().getWindow(), sf::RenderStates::Default);
	}
	//restore the gl states
	Game::instance().getWindow()->popGLStates();
	Game::instance().getWindow()->display();
}
/**.
* @brief Forwards input to widgets on the screen which may trigger a callback
* @parma event the input obtained from polling sf events
*/
void Menu::HandleInput(sf::Event event)
{
	for (int i = 0; i < m_widgets.size(); i++)
		m_widgets[i]->HandleInput(event);
}

/**.
* @brief Callback function triggered by widget. Receives the name of the button event to trigger
* and collects data from widgets to forward to global event system
* @param event name of the event
*/
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