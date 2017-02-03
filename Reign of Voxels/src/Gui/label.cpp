#include "label.h"
#include "graphics.h"

Label::Label(sf::Font * font, Widget *widget, const sf::String& string)
{
	int text_size = 32;
	float offsety = 32.0f;
	//display above the widget
	sf::Vector2f pos = widget->getPosition();
	pos.y -= offsety;

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

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	g_window->draw(m_text);
}