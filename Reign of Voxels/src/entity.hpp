#pragma once
#include "ResourceManager.hpp"
#include "Physics.hpp"
#include "States.hpp"
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
protected:
	/********Member Variables ********/
	sf::Uint8 m_flag;

	GLint m_modelID;

	//stats
	int m_health;
	int m_maxHealth;

	//update funcs
	int m_thinkTimer;
	int m_nextThink;
	int m_thinkRate;

	//phyiscs and world space locations
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_target;
	glm::vec3 m_velocity;

	float m_speed;
	float m_animStart;

	AABB m_aabb;

	State m_state;
public:
	btRigidBody *m_rigidBody;

	Entity();
	~Entity();

	virtual void Init(GLint modelID, glm::vec3 position, AABB aabb, int health, int speed, int thinkRate);
	void Destroy();

	glm::vec3 GetPosition();
	AABB GetAABB();

	virtual void Update();
	virtual void Think();

	void MoveTo(glm::vec3 target_pos);

	bool IsActive();

	void SetSelected(bool selected);
	bool IsSelected();

	int		GetHealth();
	int		GetMaxHealth();
	GLuint	GetEntityModelID();

	Entity *m_nextFree;

};