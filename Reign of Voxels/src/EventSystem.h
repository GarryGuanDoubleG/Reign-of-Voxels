#pragma once
#include <string>
#include "SFML\Graphics.hpp"
#include "Observer.h"
#include "Json.hpp"

/**
* Global event system using observer pattern
* Keeps a linked list of observers and notifies everyone subscribed to
* eventsystem
*/
class EventSystem
{
public:
	/** constructor
	* Sets head of linked list to null
	*/
	EventSystem()
		: m_head(nullptr)
	{}
	/**
	* @brief add observer to linked list
	* @param observer to subscribe to event system
	*/
	void addObserver(Observer * observer);
	/**
	* @brief remove observer from linked list
	* @param observer to unsubscribe to event system
	*/
	void removeObserver(Observer * observer);
	/**
	* @brief notifies all observers that an event has happened and passes reference to data of the event
	* @param event the event that happened
	* @param obj the json data from the event
	*/
	void Notify(Event event, Json &obj);
	/**
	* @brief notifies all observers that a user input has happened and passes reference to input
	* @param event the event that happened
	* @param input player input
	*/
	void Notify(Event event, sf::Event &input);
private:
	Observer * m_head; /**< head of linked list */  
};
