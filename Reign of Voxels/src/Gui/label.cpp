#include "label.h"
#include "game.h"

//! A constructor.
/*!
Constructor for a GUI label widget used for the menu scene
Sets inner text to empty string and sets to active
*/
Label::Label()
{
	m_text.setString("");
	m_is_active = true;
}

/*
* @brief Draws the label box and text into the sfml frame buffer. Inherited from SFML drawables base class.
* @param target the rendering target, usually window, to draw button on
* @param states the rendering state to draw with. Usually default rendering states are sent.
*/

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_is_active)
	{
		Game::instance().getWindow()->draw(m_box);		
		Game::instance().getWindow()->draw(m_text);
	}
}