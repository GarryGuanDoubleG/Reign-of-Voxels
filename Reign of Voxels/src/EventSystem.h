#pragma once
#include "Observer.h"
#include "Json.hpp"
#include <string>

using Json = nlohmann::json;
#define MAX_EVENTS 100

class Subject
{
public:
	Subject()
		: m_head(nullptr)
	{}
	void addObserver(Observer * observer);
	void removeObserver(Observer * observer);
	void Notify(Event event, Json obj);
	void Notify(Event event, std::string id); //usually used for gui events
	void Notify(Event event);
private:
	Observer * m_head;

};
//singleton event system class for broadcasting events
