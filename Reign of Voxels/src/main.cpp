/**
* @file src\main.cpp
*
* Implements the main class.
*/
#ifndef _DEBUG
#include <windows.h>
#endif
#include "game.h"
/**
* Main entry-point for this application.
*
* @author Garry Guan
*/
#ifdef _DEBUG
/**
* @param argc The number of command - line arguments provided.
* @param argv An array of command - line argument strings.
*/
int main(int argc, char *argv[])
#else

int APIENTRY WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
#endif
{
	srand(0);

	Game::instance().Initialize();
	Game::instance().GameLoop();
	return 0;
}