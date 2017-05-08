#include "structures.hpp"
#include "game.hpp"

Structure::Structure()
{
	m_structFlag = 0;
	Entity::Entity();
}

Structure::~Structure()
{

}

void Structure::Init(glm::vec3 position, AABB aabb, int health, int speed, int thinkRate, std::string name)
{
	GLuint modelID = GetModelID(name + "1");
	if (modelID == 0)
	{
		modelID = GetModelID(name);
	}
	Entity::Init(modelID, position, aabb, health, speed, thinkRate, 0, 0);

	m_structFlag = STRUCT_BUILDING_MODE; // start building

	Json def = GetStructDef(name);

	m_buildInfo = new BuildingAnimInfo;
	m_buildInfo->animStartTime = Game::clock.getElapsedTime().asSeconds();
	m_buildInfo->buildFrame = 1;
	m_buildInfo->maxFrame = def["totalFrames"];
	m_buildInfo->tickRate = def["tick_rate"];
	m_buildInfo->name = name;
}

void Structure::Update()
{
	if (m_structFlag & STRUCT_BUILDING_MODE)
	{
		float delta_time = Game::clock.getElapsedTime().asSeconds() - m_buildInfo->animStartTime;
		int buildFrame = delta_time / m_buildInfo->tickRate;

		if (buildFrame > m_buildInfo->maxFrame)
		{
			m_structFlag &= ~STRUCT_BUILDING_MODE;
			m_modelID = GetModelID(m_buildInfo->name);

			delete m_buildInfo;
			m_buildInfo = NULL;
		}
		else if (m_buildInfo->buildFrame != buildFrame)
		{
			m_buildInfo->buildFrame = buildFrame;
			std::string nextModel = m_buildInfo->name + std::to_string(buildFrame);
			m_modelID = GetModelID(nextModel);
		}		
	}

	if (m_health < 0)
	{
		Destroy();
	}

}

void Structure::Think()
{

}