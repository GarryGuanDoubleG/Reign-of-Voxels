#include "label.h"
#include "graphics.h"
#define OFFSETY 32.0f

Label::Label(sf::Font * font, Widget *widget, const sf::String& string, bool subheading)
{
	int text_size = 32;
	//display above the widget
	sf::Vector2f pos = widget->getPosition();
	pos.y -= OFFSETY; //if sub then go below else above

	m_text.setString(string);
	m_text.setFillColor(sf::Color::Red);
	m_text.setFont(*font);
	m_text.setStyle(sf::Text::Bold);
	m_text.setPosition(pos);
	m_text.setCharacterSize(text_size);

	setPosition(pos);
}

void Label::setText(const sf::String &string)
{
	m_text.setString(string);
}

void Label::setTextSize(size_t size)
{
	m_text.setCharacterSize(size);
}

void Label::setPosition(sf::Vector2f position)
{
	m_position = position;
	m_text.setPosition(position);
}

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(is_active)
		g_window->draw(m_text);
}

void Label::setSubLabel()
{
	sf::Vector2f pos = getPosition();
	pos.y -= 2 * OFFSETY;
	setPosition(pos);
}

bool Label::isSelectable()
{
	return false;
}

void Label::setColor(const sf::Color &color)
{
	m_text.setFillColor(color);
}
const sf::Color& Label::getColor() const
{
	return m_text.getFillColor();
}