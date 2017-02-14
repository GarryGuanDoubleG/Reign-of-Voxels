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
protected:
	/*
	* @brief Draws the label box and text into the sfml frame buffer. Inherited from SFML drawables base class.
	* @param target the rendering target, usually window, to draw button on
	* @param states the rendering state to draw with. Usually default rendering states are sent.
	*/
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
