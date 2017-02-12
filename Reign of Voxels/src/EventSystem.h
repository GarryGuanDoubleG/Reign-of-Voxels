#pragma once
#include <string>
#include "SFML\Graphics.hpp"
#include "Observer.h"
#include "Json.hpp"

//event system
class EventSystem
{
public:
	EventSystem()
		: m_head(nullptr)
	{}
	void addObserver(Observer * observer);
	void removeObserver(Observer * observer);
	void Notify(Event event, Json &obj);
	void Notify(Event event, sf::Event &input);
private:
	Observer * m_head;
};
