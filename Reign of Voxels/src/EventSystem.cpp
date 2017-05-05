#include "EventSystem.hpp"
/**
* @brief add observer to linked list
* @param observer to subscribe to event system
*/
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
/**
* @brief remove observer from linked list
* @param observer to unsubscribe to event system
*/
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
/**
* @brief notifies all observers that an event has happened and passes reference to data of the event
* @param event the event that happened
* @param obj the json data from the event
*/
void EventSystem::Notify(Event event, Json &json_obj)
{
	Observer * current = m_head;

	while (current)
	{
		current->onNotify(event, json_obj);
		current = current->m_next;
	}
}
/**
* @brief notifies all observers that a user input has happened and passes reference to input
* @param event the event that happened
* @param input player input
*/
void EventSystem::Notify(Event event, sf::Event &input)
{
	Observer * current = m_head;

	while (current)
	{
		current->onNotify(event, input);
		current = current->m_next;
	}
}


void EventSystem::Notify(Event event, std::string &input)
{
	Observer * current = m_head;

	while (current)
	{
		current->onNotify(event, input);
		current = current->m_next;
	}
}