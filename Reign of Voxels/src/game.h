#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include "graphics.h"
#include "EventSystem.h"
#include "glm.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Gui\Menu.h"
#include "server.h"
#include "client.h"

class Game
{
public:

	Game();
	static Game&	instance() { return m_instance; }
	void			Initialize();
	void			GameLoop();

	Subject&		getEventSystem() { return *m_eventSystem; }
	
private:
	static Game m_instance;

	Subject		*m_eventSystem;
	Client		*m_client;
	Server		*m_server;

	bool		m_initialized;
	bool		m_is_client;
};

#endif