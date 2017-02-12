#include "EventSystem.h"

void EventSystem::addObserver(Observer * observer)
{
	observer->m_prev = nullptr;
	observer->m_next = m_head;

	if (m_head)
	{
		m_head->m_prev = observer;
	}
	m_head = observer;	
}

void EventSystem::removeObserver(Observer * observer)
{
	if (m_head == observer)
	{
		m_head = observer->m_next;
	}
	else
	{
		observer->m_prev->m_next = observer->m_next;
	}
}

void EventSystem::Notify(Event event, Json &json_obj)
{
	Observer * current = m_head;

	while (current)
	{
		current->onNotify(event, json_obj);
		current = current->m_next;
	}
}
