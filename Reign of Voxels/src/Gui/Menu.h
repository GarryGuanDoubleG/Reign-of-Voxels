#pragma once
#include "SFML\Graphics.hpp"
#include "command.h"
#include "Gui\Textbox.h"

//list of states to know which to render to
//do states even make sense idk
enum MenuState
{
	STATE_TEXTBOX,
	STATE_IDLE
};

class MenuCommand : public Command
{
public: 
	virtual void execute(Actor *actor) = 0;
	virtual void execute()
	{

	}
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
	std::string login_name_;
	MenuState state_;

	sf::Font font_;
	sf::Text text_;

	TextBox box;
};