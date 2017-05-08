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
#define ENTITY_ATTACK 16

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
	float m_health;
	float m_maxHealth;

	//update funcs
	int m_thinkTimer;
	int m_nextThink;
	int m_thinkRate;

	//phyiscs and world space locations
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_velocity;
	glm::vec3 m_forward;

	float m_speed;
	float m_animStart;
	float m_attack;
	float m_range;

	float m_deathTimer;

	AABB m_aabb;

	State m_state;
public:
	btRigidBody *m_rigidBody;
	std::string m_stateStr;
	Entity *m_enemy;
	glm::vec3 m_target;
	bool m_boss;

	Entity();
	~Entity();

	virtual void Init(GLint modelID, glm::vec3 position, AABB aabb, int health, int speed, int thinkRate, int range, int attackDmg);
	void Destroy();

	glm::vec3 GetPosition();
	AABB GetAABB();

	virtual void Update();
	virtual void Think();
	virtual void Attack(Entity *enemy);


	void MoveTo(glm::vec3 target_pos);

	bool IsActive();

	void SetSelected(bool selected);
	bool IsSelected();

	float		GetHealth();
	float		GetMaxHealth();
	GLuint	GetEntityModelID();

	Entity *m_nextFree;

};