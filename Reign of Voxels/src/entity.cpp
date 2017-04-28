#include "game.hpp"
#include "entity.hpp"

Entity::Entity()
{
	m_flag = 0;
}
Entity::~Entity()
{

}

void Entity::Init(GLint modelID, glm::vec3 position, AABB aabb)
{
	m_flag = 0;
	m_flag |= ENTITY_ACTIVE;

	m_modelID = modelID;
	m_position = position;

	m_speed = 1.0f;

	m_aabb = aabb;

	//MoveTo(glm::vec3(256.0f, 32.0f, 256.0f));
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

AABB Entity::GetAABB()
{
	return m_aabb;
}

GLuint Entity::GetModelID()
{
	return m_modelID;
}


void Entity::Update()
{
	if (m_flag & ENTITY_MOVING)
	{
		m_velocity = glm::normalize(m_target - m_position) * m_speed;
		if (glm::distance(m_position, m_target) > 1.0f)
		{
			m_position += m_velocity;
		}			
		else
		{
			m_position = m_target;
			m_flag &= ~ENTITY_MOVING;
		}
	}

	m_thinkTimer += Game::delta_time;

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
	m_target = target_pos;
	m_flag |= ENTITY_MOVING;
}


bool Entity::IsActive()
{
	return m_flag & ENTITY_ACTIVE;
}

void Entity::SetSelected(bool selected)
{
	if (selected)
		m_flag |= ENTITY_SELECTED;
	else
		m_flag &= ~ENTITY_SELECTED;
}

bool Entity::IsSelected()
{
	return m_flag & ENTITY_SELECTED;
}