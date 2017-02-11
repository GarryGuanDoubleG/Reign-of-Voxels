#pragma once
#include "SFML\Graphics.hpp"
#include "layout.h"
#include "textbox.h"
#include "scene.h"
#include "game.h"
#include "json.hpp"

using Json = nlohmann::json;

/*
	setting up this menu for login only right now
	will later make a scene class that menu will extend from
	and a scene manager, this way I can push and pop scenes for easy
	management
*/
class Menu : virtual public Scene
{
public:
	Menu(MenuLayouts layout);
	Menu(MenuLayouts layout, Json &data);
	~Menu();
	
	virtual void Render();
	virtual void SceneFrame();
	virtual void HandleInput(sf::Event event);
	virtual void triggerCallBack(sf::String event);		

	virtual void onNotify(Event event, Json &data);

	void UpdateWidgets(Json &data);
private:
	std::vector<Widget*> m_widgets;
	Layout *m_layout;
};