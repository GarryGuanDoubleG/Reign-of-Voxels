#pragma once
#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "Actor.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(Actor* actor) = 0;
};
#endif