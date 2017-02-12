#include "label.h"
#include "game.h"

#define OFFSETY 32.0f

Label::Label()
{
	m_text.setString("");
	m_is_active = true;
}

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_is_active)
	{
		Game::instance().getWindow()->draw(m_box);		
		Game::instance().getWindow()->draw(m_text);
	}
}

void Label::setSubLabel()
{
	sf::Vector2f pos = getPosition();
	pos.y -= 2 * OFFSETY;
	setPosition(pos);
}