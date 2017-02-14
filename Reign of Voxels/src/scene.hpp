#pragma once
#include "EventSystem.hpp"

#define MAX_SCENES 10
#define MAX_PENDING_EVENTS 100
/**
* Abstract class that manages what renders to the screen and how to handles input
*/
class Scene : public Observer
{
public:
	/**
	* Destructor
	*/
	virtual ~Scene() {};
	/**
	* @brief Manages what occurs each frame of the game loop
	*/
	virtual void SceneFrame() = 0;
	/**
	* @brief draws to frame buffer
	*/
	virtual void Render() = 0;
	/**
	* @brief Manages what happens on user input
	* @param event user input
	*/
	virtual void HandleInput(sf::Event event) = 0;
};