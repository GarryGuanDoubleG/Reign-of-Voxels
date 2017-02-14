#include "button.h"

//! A constructor.
/*!
Constructor for a GUI button widget used for the menu scene
Sets inner text to empty string
*/
Button::Button()
{
	m_text.setString("");
}

/*
* @brief Draws the button box and text into the sfml frame buffer. Inherited from SFML drawables base class.
* @param target the rendering target, usually window, to draw button on
* @param states the rendering state to draw with. Usually default rendering states are sent.
*/
void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(m_box);
	target.draw(m_text);
}
/*
* @brief Checks if button is clicked and triggers GUI callbacks. Inherited from widget base class.
* @param x the x coordinate of the mouse click
* @param y the y coordinate of the mouse click
*/
void Button::onMouseEntered(float x, float y)
{
	sf::Vector2f pos = m_box.getPosition();
	float width = m_box.getSize().x;
	float height = m_box.getSize().y;

	//if button click, trigger callbacks
	if (CHECKBOUNDS(x, y, pos.x, pos.y, width, height))
	{
		triggerCallBack();
	}

}

