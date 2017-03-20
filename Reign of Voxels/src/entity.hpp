#pragma once
#include "SFML\Graphics.hpp"

#include "glm.hpp"

//flags
#define ENTITY_ACTIVE 1
#define ENTITY_SELECTED 2
#define ENTITY_MOVING 4

//entity type
#define ENTITY_AIR 8

/**
*@brief axis aligned bounding box
*/
typedef struct 
{
	glm::vec3 start_offset; /*<offset from entity position to start bounding box*/
	glm::vec3 size; /** dimensions of bounding box*/
}BoundingBox;

class Entity
{
public:
	Entity();
	~Entity();

	void Init(GLint modelID, glm::vec3 position);
	void Destroy();

	glm::vec3 GetPosition();
	GLuint GetModelID();

	void Update();

	void MoveTo(glm::vec3 target_pos);
	

	bool IsActive();

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

	BoundingBox bounds;
};