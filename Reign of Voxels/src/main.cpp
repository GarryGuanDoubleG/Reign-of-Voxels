/**
* @file src\main.cpp
*
* Implements the main class.
*/
#ifndef _DEBUG
#include <windows.h>
#endif

#include "game.h"

#ifdef _DEBUG

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
	bool is_server = false;;
	if (argc > 1)
	{
		if (argv[1] != "")
		{
			is_server = true;
		}
	}

	Game::instance().Initialize(is_server);
	Game::instance().GameLoop();
	return 0;
}