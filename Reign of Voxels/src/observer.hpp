#pragma once
#include "events.hpp"
#include "json.hpp"
#include "entity.hpp"

using Json = nlohmann::json; /**< convenience specified by nlohmann himself */  
/**
* observer class that listens for event calls
*/
class Observer
{
	friend class EventSystem;
public: 
	/** Constructor
	* Sets members to null
	*/
	Observer()
		: m_next(nullptr),
		m_prev(nullptr)
	{};
	/** 
	* @brief Responds to global event occurring and manages relevant data
	* @param event event happened
	* @param obj the json data associated with it
	*/
	virtual void onNotify(Event event, Json &obj) {};
	/**
	* @brief Responds to global input event
	* @param event event happened
	* @param input the input given
	*/
	virtual void onNotify(Event event, sf::Event &input) {};

	virtual void onNotify(Event event, Entity *entity) {};
private:
	//doubly ll for constant time removal
	Observer * m_next; /**< next observer to notify*/  
	Observer * m_prev; /**< previous observer to notify*/  
};