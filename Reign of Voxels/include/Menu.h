#pragma once

#include "command.h"

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

};