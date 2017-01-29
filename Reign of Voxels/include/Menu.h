#pragma once

#include "command.h"
#include "SFML\Graphics.hpp"
class MenuCommand : public Command
{
public: 
	virtual void execute(Actor *actor) = 0;
	virtual void execute()
	{

	}
};

class Menu
{
	std::string player_name;
	sf::Font font;
	sf::Text login_text;

	Menu();
	~Menu();

	void RenderLoginMenu();
	void LoginMenuLoop();

	void Lobby();
};