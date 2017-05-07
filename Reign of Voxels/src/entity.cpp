#include "entity.hpp"
#include "game.hpp"

Entity::Entity()
{
	m_flag = 0;
}
Entity::~Entity()
{

}

void Entity::Init(GLint modelID, glm::vec3 position, AABB aabb, int health, int speed, int thinkRate)
{
	m_flag = 0;
	m_flag |= ENTITY_ACTIVE;

	m_modelID = modelID;
	m_position = position;

	m_speed = speed;
	m_thinkRate = thinkRate;

	m_aabb = aabb;

	m_health = 100;
	m_maxHealth = 100;
}

void Entity::Destroy()
{
	//clear flags
	m_flag = m_flag >> 8;
}

glm::vec3 Entity::GetPosition()
{
	return m_position;
}

AABB Entity::GetAABB()
{
	return m_aabb;
}

GLuint Entity::GetEntityModelID()
{
	return m_modelID;
}
int Entity::GetHealth()
{
	return m_health;
}
int	Entity::GetMaxHealth()
{
	return m_maxHealth;
}

void Entity::Update()
{
	btVector3 newPosition;
	newPosition = m_rigidBody->getWorldTransform().getOrigin();

	//TODO optimize this to not use glm
	glm::vec3 position = glm::vec3(newPosition.getX(), newPosition.getY(), newPosition.getZ());
	m_position = position;

	if (m_flag & ENTITY_MOVING)
	{
		if (glm::distance(position, m_target) < .67f)
		{
			
			m_rigidBody->setLinearVelocity(btVector3(0, 0, 0));
			m_position = m_target;
			m_flag &= ~ENTITY_MOVING;

			m_state = idle;
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
	glm::vec3 direction = glm::normalize(target_pos - m_position);
	btVector3 velocity = btVector3(direction.x, direction.y *.5f, direction.z) * m_speed;


	m_rigidBody->activate(true);
	m_rigidBody->setLinearVelocity(velocity);
	//m_rigidBody->setLinearFactor(velocity * .3f);
	m_target = target_pos;
	m_flag |= ENTITY_MOVING;

	m_state = walking;
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