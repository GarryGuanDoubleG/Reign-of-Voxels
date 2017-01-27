#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include "graphics.h"

typedef struct
{
	sf::Font quicksand_reg;
	sf::Font quicksand_bold;
	sf::Font quicksand_italic;
}Font;


void Initialize();
void GameLoop();
#endif