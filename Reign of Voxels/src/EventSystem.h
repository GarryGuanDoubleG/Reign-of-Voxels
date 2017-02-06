#pragma once
#include "json.hpp"
#include <string>

using json = nlohmann::json;

enum Event
{
	//Gui Events
	onButton
	//Game events
};

class Subject;

class Observer
{
	friend class Subject;
public: Observer() 
	: m_next(nullptr)
	{}
	virtual void onNotify(Event event, json obj) = 0;
	virtual void onNotify(Event event, std::string id);
private:
	//doubly ll for constant time removal
	Observer * m_next;
	Observer * m_prev;
};
class Subject
{
public:
	Subject()
		: m_head(nullptr)
	{}

	void addObserver(Observer * observer);
	void removeObserver(Observer * observer);
	void Notify(Event event, json obj);
	void Notify(Event event, std::string id); //usually used for gui events
private:
	Observer * m_head;
};
//singleton event system class for broadcasting events
class EventSystem : public Subject
{
public:
	static EventSystem &instance();
};