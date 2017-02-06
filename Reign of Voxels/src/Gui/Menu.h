#pragma once
#include "SFML\Graphics.hpp"
#include "EventSystem.h"
#include "layout.h"
#include "textbox.h"

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
	virtual void triggerCallBack(sf::String event);
private:
	std::vector<Widget*> m_widgets;
	Layout *m_layout;
};