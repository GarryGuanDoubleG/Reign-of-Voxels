#pragma once
#ifndef _GAME_H_
#define _GAME_H_

#include "graphics.h"
#include "glm.h"
#include "client.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "Gui\Menu.h"
#include "EventSystem.h"

class Game
{
public:
	static Game& instance() { return m_instance; }

	Subject& getEventSystem() { return m_eventsystem; }
private:
	static Game m_instance;

	Subject * m_eventsystem;
};
void Initialize();
void GameLoop();
#endif