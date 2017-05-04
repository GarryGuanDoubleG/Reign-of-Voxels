#pragma once
#include "entity.hpp"

#define MAX_PENDING 8
#define STRUCT_BUILDING_MODE 1
#define STRUCT_PRODUCING_MODE 2

typedef struct
{
	float animStartTime;
	float tickRate;

	int buildFrame;
	int maxFrame;
	std::string name;
}BuildingAnimInfo;

class Structure : public Entity
{
	GLuint m_pending[MAX_PENDING]; /*<max number of entities that can get queued up*/

	sf::Uint8 m_structFlag;
	BuildingAnimInfo *m_buildInfo;
public:
	Structure *m_nextFree;

	Structure();
	~Structure();

	void Update();
	void Init(glm::vec3 position, AABB aabb, int health, int speed, int thinkRate, std::string name);

private:
	void Think();
};