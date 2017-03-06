/**
* @file src\main.cpp
*
* Implements the main class.
*/

#include <windows.h>
#include "game.hpp"
/**
* Main entry-point for this application.
*
* @author Garry Guan
*/

/**
* @param argc The number of command - line arguments provided.
* @param argv An array of command - line argument strings.
*/

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