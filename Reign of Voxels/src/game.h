#pragma once
#ifndef _GAME_H_
#define _GAME_H_
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
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
	~Game();

	void					Initialize();
	void					GameLoop();
	static Game&			instance();
	EventSystem&			getEventSystem();
	sf::RenderWindow*	getWindow();
private:

	void					GraphicsInit();
	void					Close();

	static Game				m_instance;
	sf::RenderWindow		*m_window;
	
	//managers
	SceneManager			*m_sceneManager;
	EventSystem				*m_eventSystem;
	Client					*m_client;

	//flags
	bool					m_running;
	bool					m_initialized;
	bool					m_is_client;
};

#endif