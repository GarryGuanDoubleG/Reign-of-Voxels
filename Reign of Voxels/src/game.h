#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include "graphics.h"
#include "EventSystem.h"
#include "glm.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "scene.h"
#include "Gui\Menu.h"
#include "client.h"

class Game
{
public:

	Game();
	static Game&	instance() { return m_instance; }
	void			Initialize();
	void			Close();
	void			GameLoop();

	EventSystem&		getEventSystem() { return *m_eventSystem; }
	
private:
	static Game m_instance;

	SceneManager	*m_sceneManager;
	EventSystem		*m_eventSystem;
	Client			*m_client;

	bool		m_running;
	bool		m_initialized;
	bool		m_is_client;
};

#endif