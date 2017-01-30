#pragma once
#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "Actor.h"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

/*
*	Virtual class to run specific commands on input
* Architected to run commands with polymorphism
*/
class Command
{
public:
	sf::String text;

	virtual ~Command() {}
	virtual void execute(Actor* actor) = 0;
	virtual void execute() = 0;
};

class InputHandler
{
public:
	Command * HandleInput(sf::Event event)
	{
		switch (event.mouseButton.button)
		{
		case sf::Mouse::Left:
			return cmdMouseLeft;			
		case sf::Mouse::Right:
			return cmdMouseRight;
		}
		switch (event.key.code)
		{
		case sf::Keyboard::Return:
			return cmdEnter;
		case sf::Keyboard::Escape:
			return cmdEsc;
		case sf::Keyboard::Space:
			return cmdSpace;
		case sf::Keyboard::BackSpace:
			return cmdBackspace;			
		default:
			break;
		}

		if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode < 128)
			{
				if (cmdText)
				{
					cmdText->text += static_cast<char>(event.text.unicode);
				}
				return cmdText;
			}
		}
		return NULL;
	}
private:
	Command *cmdEnter;
	Command *cmdEsc;
	Command *cmdSpace;
	Command *cmdMouseLeft;
	Command *cmdMouseRight;
	Command *cmdBackspace;
	Command *cmdNumber;
	Command *cmdText;
};

class TextCommand : public Command
{
public:
	virtual void execute()
	{
		
	}
};
#endif