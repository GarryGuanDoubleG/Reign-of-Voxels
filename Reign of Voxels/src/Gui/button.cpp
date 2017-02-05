#include "button.h"

Button::Button(sf::Font * font, const sf::String &string, sf::Vector2f position, float width, float height)
{
	m_text.setFont(*font);
	m_text.setCharacterSize(24);
	m_text.setFillColor(sf::Color::Red);
	m_text.setString(string);
	
	m_box.setSize(sf::Vector2f(width, height));
	setPosition(position);
}

void Button::setPosition(sf::Vector2f position)
{
	sf::Vector2f padding = m_box.getSize();
	padding.x = padding.x * .33;
	padding.y = padding.y * .33;

	m_box.setPosition(position);
	m_position = position;

	m_text.setPosition(position + padding);
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

	//if button click, trigger callbacks
	if (CHECKBOUNDS(x, y, pos.x, pos.y, width, height))
	{

	}
}
