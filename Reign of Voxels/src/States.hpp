#pragma once
#include "entity.hpp"

class EntityState
{
public:
	EntityState();

	virtual void Update(Entity *entity);
	virtual void HandleInput(sf::Event input);


};