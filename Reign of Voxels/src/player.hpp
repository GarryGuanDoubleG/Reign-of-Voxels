#pragma once
#include "camera.hpp"
#include "PhysicsUtil.hpp"

class Player
{
public:
	void HandleInput(sf::Event &event);
	
private:
	sf::Uint8 m_flag;

	Camera *m_camera;

	//TODO move to a player class
	glm::ivec3 m_selection_box;
	glm::ivec3 m_selection_face;
};