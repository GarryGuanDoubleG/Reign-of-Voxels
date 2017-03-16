#pragma once
#include <GL\glew.h>
#include <SFML\OpenGL.hpp>

#include "camera.hpp"
#include "glm.hpp"

class HUD
{
public:
	HUD();

	void DrawMiniMap();
	void HandleInput(sf::Event event);

private:
	Camera *m_minimapCam;

};