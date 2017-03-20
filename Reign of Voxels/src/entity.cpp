#include "game.hpp"
#include "entity.hpp"

Entity::Entity()
{
	m_flag = 0;
}
Entity::~Entity()
{

}

void Entity::Init(GLint modelID, glm::vec3 position)
{
	m_flag >> 8;
	m_flag |= ENTITY_ACTIVE;

	m_modelID = modelID;
	m_position = position;

	m_speed = 1.0f;

	MoveTo(glm::vec3(256.0f, 256.0f, 256.0f));
}

void Entity::Destroy()
{
	//clear flags
	m_flag >> 8;
}

glm::vec3 Entity::GetPosition()
{
	return m_position;
}
GLuint Entity::GetModelID()
{
	return m_modelID;
}


void Entity::Update()
{
	if ((m_flag & ENTITY_MOVING) == ENTITY_MOVING)
	{
		m_velocity = glm::normalize(m_target - m_position) * m_speed;
		if (glm::distance(m_position, m_target) > 3.0f)
		{
			m_position += m_velocity;
		}			
		else
		{
			m_flag &= ~ENTITY_MOVING;
		}
	}

	m_thinkTimer += Game::g_delta_clock.getElapsedTime().asSeconds();

	if (m_thinkTimer >= m_nextThink)
	{
		m_nextThink += m_thinkRate;
		Think();
	}

}

void Entity::Think()
{

}

void Entity::MoveTo(glm::vec3 target_pos)
{
	//if (m_flag & ENTITY_AIR)
	//{
	//	m_velocity = glm::normalize(target_pos - m_position) * m_speed;
	//	m_target = target_pos;
	//}
	//else
	//{
	//	m_velocity = glm::normalize(target_pos - m_position) * m_speed;
	//	m_target = glm::vec3(target_pos.x, 0, target_pos.z);
	//}

	m_target = glm::vec3(target_pos.x, 0, target_pos.z);
	m_flag |= ENTITY_MOVING;
}


bool Entity::IsActive()
{
	return m_flag & ENTITY_ACTIVE;
}