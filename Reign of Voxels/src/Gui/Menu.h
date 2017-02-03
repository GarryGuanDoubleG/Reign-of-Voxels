#pragma once
#include "SFML\Graphics.hpp"
#include "command.h"
#include "textbox.h"

//list of states to know which to render to
//do states even make sense idk
enum MenuState
{
	STATE_IDLE,
	STATE_WIDGET
};

/*
	setting up this menu for login only right now
	will later make a scene class that menu will extend from
	and a scene manager, this way I can push and pop scenes for easy
	management
*/
class Menu
{
public:
	Menu();
	~Menu();
	
	virtual void Render();
	virtual void MenuLoop();
	virtual void HandleInput(sf::Event event);

private:
	MenuState state_;
	sf::Font *m_font;
	std::vector<Widget> m_widgets;
};