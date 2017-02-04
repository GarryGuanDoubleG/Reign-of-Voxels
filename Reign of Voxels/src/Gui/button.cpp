#include "button.h"

Button::Button(sf::Font * font, sf::String &string, sf::Vector2f position, float width, float height)
{
	m_text.setFont(*font);
	m_text.setCharacterSize(48);
	m_text.setFillColor(sf::Color::Red);
	m_text.setString(string);
	setPosition(position);
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(m_box);
	target.draw(m_text);
}

void Button::setString(const sf::String &string)
{
	m_text.setString(string);
}

void Button::onMouseEntered(float x, float y)
{
	sf::Vector2f pos = m_box.getPosition();
	float width = m_box.getSize().x;
	float height = m_box.getSize().y;

	if (CHECKBOUNDS(x, y, pos.x, pos.y, width, height))
	{
	}
}