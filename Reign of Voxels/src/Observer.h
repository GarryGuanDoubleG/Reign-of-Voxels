#pragma once
#include "events.h"
#include "json.hpp"
using Json = nlohmann::json;

class Observer
{
	friend class EventSystem;
public: 
	Observer()
		: m_next(nullptr),
		m_prev(nullptr)
	{};
		virtual void onNotify(Event event, Json &obj) {};
private:
	//doubly ll for constant time removal
	Observer * m_next;
	Observer * m_prev;
};