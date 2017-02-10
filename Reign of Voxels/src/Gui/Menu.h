#pragma once
#include "SFML\Graphics.hpp"
#include "layout.h"
#include "textbox.h"
#include "scene.h"
#include "game.h"

/*
	setting up this menu for login only right now
	will later make a scene class that menu will extend from
	and a scene manager, this way I can push and pop scenes for easy
	management
*/
class Menu : virtual public Scene
{
public:
	Menu(MenuLayouts layout)
		: m_layout()
	{
		m_widgets = m_layout->LoadMenuLayout(layout);
		//set parent for callback
		//seems like some coupling but good enough?
		for (int i = 0; i < m_widgets.size(); i++)
		{
			m_widgets[i]->setParent(this);
		}
	};
	~Menu();
	
	virtual void Render();
	virtual void SceneLoop();
	virtual void HandleInput(sf::Event event);
	virtual void triggerCallBack(sf::String event);
private:
	std::vector<Widget*> m_widgets;
	Layout *m_layout;
};