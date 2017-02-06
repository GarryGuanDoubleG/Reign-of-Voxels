#include "EventSystem.h"


void Observer::onNotify(Event event, json obj)
{

}

void Observer::onNotify(Event event, std::string id)
{

}

void Subject::addObserver(Observer * observer)
{
	observer->m_prev = nullptr;
	observer->m_next = m_head;

	m_head->m_prev = observer;
	m_head = observer;	
}

void Subject::removeObserver(Observer * observer)
{
	if (m_head == observer)
	{
		m_head = observer->m_next;
	}
	else
	{
		observer->m_prev->m_next = observer->m_next;
		delete(observer);
	}
}

void Subject::Notify(Event event, json json_obj)
{
	Observer * current = m_head;

	while (current->m_next)
	{
		current->onNotify(event, json_obj);
		current = current->m_next;
	}
}

void Subject::Notify(Event event, std::string id)
{
	Observer * current = m_head;

	while (current->m_next)
	{
		current->onNotify(event, id);
		current = current->m_next;
	}
}