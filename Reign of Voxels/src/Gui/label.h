#pragma once
#include "widget.h"
/**
* Displays text and color box at specified window position
*/
class Label : public Widget
{
public:
	//! A constructor.
	/*!
	Constructor for a GUI label widget used for the menu scene
	Sets inner text to empty string and sets to active
	*/
	Label();
	const sf::String& getText() const;

	/*
	* @brief Checks if button is clicked and triggers GUI callbacks. Inherited from widget base class.
	* @param x the x coordinate of the mouse click
	* @param y the y coordinate of the mouse click
	*/
	void Label::setSubLabel();//set it to small text below assigned widget
protected:
	/*
	* @brief Draws the label box and text into the sfml frame buffer. Inherited from SFML drawables base class.
	* @param target the rendering target, usually window, to draw button on
	* @param states the rendering state to draw with. Usually default rendering states are sent.
	*/
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
