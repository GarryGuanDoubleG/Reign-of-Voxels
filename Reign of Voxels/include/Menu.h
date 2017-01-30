#pragma once
#include "command.h"
#include "SFML\Graphics.hpp"

//list of states to know which to render to
//do states even make sense idk
enum MenuState
{
	STATE_LOGIN,
	STATE_LOBBY,
	STATE_GAME
};

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
public:
	Menu();
	~Menu();

	//use input handler based on current state
	static InputHandler GameHandler;
	static InputHandler LoginHandler;
	static InputHandler LobbyHandler;

private:
	std::string login_name_;
	MenuState state_;

	sf::Font font_;
	sf::Text text_;

	void RenderLoginMenu();
	void LoginMenuLoop();
	void Lobby();
};