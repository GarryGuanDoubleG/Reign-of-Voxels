#pragma once
#include "json.hpp"

using Json = nlohmann::json;

enum Event
{
	Error,
	Login,
	JoinLobby,
	Input,
	Chat,
	Start
};

#define MAX_EVENTS
class Observer
{
	friend class Subject;
public: Observer()
	: m_next(nullptr)
{}
		virtual void onNotify(Event event, Json obj) = 0;
		virtual void onNotify(Event event, std::string id);
private:
	//doubly ll for constant time removal
	Observer * m_next;
	Observer * m_prev;
};