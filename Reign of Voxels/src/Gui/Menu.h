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
class Menu : public Scene
{
public:
	Menu();
	~Menu();
	
	virtual void Render();
	virtual void SceneLoop();
	virtual void HandleInput();
	virtual void triggerCallBack(sf::String event);
private:
	std::vector<Widget*> m_widgets;
	Layout *m_layout;
};