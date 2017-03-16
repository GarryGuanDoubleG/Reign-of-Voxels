#pragma once
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>
#include "camera.hpp"
#include "glm.hpp"

#define HUD_PATH "Resources/gui/HUD.json"

class HUD
{
public:
	HUD();

	void HUD::Init();

	void DrawMiniMap();
	void HandleInput(sf::Event event);	
private:
	Camera *m_minimapCam;

};