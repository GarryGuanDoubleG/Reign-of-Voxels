#pragma once
#include "SFML\Graphics.hpp"

#include "3dmath.hpp"

//flags
#define ENTITY_ACTIVE 1
#define ENTITY_SELECTED 2
#define ENTITY_MOVING 4

//entity type
#define ENTITY_AIR 8

/**
*@brief axis aligned bounding box
*/

class Entity
{
public:
	Entity();
	~Entity();

	void Init(GLint modelID, glm::vec3 position, AABB aabb);
	void Destroy();

	glm::vec3 GetPosition();
	AABB GetAABB();

	GLuint GetModelID();

	void Update();

	void MoveTo(glm::vec3 target_pos);
	

	bool IsActive();

	void SetSelected(bool selected);
	bool IsSelected();

	Entity *m_nextFree;
private:
	void Think();

	/********Member Variables ********/
	sf::Uint8 m_flag;

	GLint m_modelID;

	//stats
	int m_health;

	//update funcs
	int m_thinkTimer;
	int m_nextThink;
	int m_thinkRate;

	//phyiscs and world space locations
	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_velocity;

	float m_speed;

	AABB m_aabb;
};