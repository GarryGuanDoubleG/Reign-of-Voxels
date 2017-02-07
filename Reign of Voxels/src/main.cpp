/**
* @file src\main.cpp
*
* Implements the main class.
*/
#include <windows.h>
#include "game.h"



int APIENTRY WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
{
	srand(0);

	Game::instance().Initialize();
	Game::instance().GameLoop();
	return 0;
}