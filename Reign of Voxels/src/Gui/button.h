#pragma once
#include "widget.h"
/**
*  A test class. A more elaborate class description.
*/
class Button : public Widget
{
public:
	//! A constructor.
	/*!
		Constructor for a GUI button widget used for the menu scene
		Sets inner text to empty string
	*/
	Button::Button();
	/*
	* @brief Checks if button is clicked and triggers GUI callbacks. Inherited from widget base class.
	* @param x the x coordinate of the mouse click
	* @param y the y coordinate of the mouse click
	*/
	virtual void onMouseEntered(float x, float y); 
private:
	/*
	* @brief Draws the button box and text into the sfml frame buffer. Inherited from SFML drawables base class.
	* @param target the rendering target, usually window, to draw button on
	* @param states the rendering state to draw with. Usually default rendering states are sent.
	*/
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};