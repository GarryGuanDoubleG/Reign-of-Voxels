#include <GL\glew.h>
#include "Gui\Textbox.h"
#include "graphics.h"
#include "simple_logger.h"

#define CURSORPERIOD .75f

TextBox::TextBox(sf::Font *font, sf::Vector2f position, float height, float width)
{
	int border_size = 2;
	float color[] = { 1.0f, 1.0f, 1.0f };
	int text_size = 24;
	m_box.setSize(sf::Vector2f(width, height));
	//set position
	setPosition(position);
	//padding
	m_padding = text_size / 2;
	//text
	m_text.setFillColor(sf::Color::Red);
	m_text.setFont(*font);
	m_text.setStyle(sf::Text::Bold);
	m_text.setString("");
	m_text.setPosition(sf::Vector2f(position.x + m_padding, position.y + m_padding));
	m_text.setCharacterSize(text_size);
	//cursor color
	m_cursor.setFillColor(sf::Color::Red);
	m_cursor.setPosition(sf::Vector2f(position.x + (float)border_size, position.y)); //set it to the left of textbox
	m_cursor.setSize(sf::Vector2f(4, text_size));
	//string
	m_string.clear();
	m_max_text_length = 16;
}

void TextBox::onKeyPressed(sf::Keyboard::Key key)
{
	if (!m_selected)
		return;
	switch (key)
	{
	case sf::Keyboard::Right:
		if (m_cursor_idx < m_string.getSize())
			setCursor(m_cursor_idx + 1);
		break;
	case sf::Keyboard::Left:
		if (m_cursor_idx > 0)
			setCursor(m_cursor_idx - 1);
		break;
	case sf::Keyboard::BackSpace:
		if (m_cursor_idx > 0)
		{
			m_string.erase(m_cursor_idx - 1);
			setCursor(m_cursor_idx - 1);
		}
		break;
	default:
		break;
	}
	m_text.setString(m_string);
}

void TextBox::onTextEntered(sf::Uint32 unicode)
{
	if (!m_selected) return;

	if (isalpha(unicode) || isalnum(unicode))
	{
		if (m_string.getSize() < m_max_text_length)
		{
			m_string.insert(m_cursor_idx,static_cast<char>(unicode));
			m_text.setString(m_string);
			setCursor(m_cursor_idx + 1);
		}
	}
}

void TextBox::onMouseEntered(float x, float y)
{
	//textbox size
	sf::Vector2f pos(m_box.getPosition());
	float width = m_box.getSize().x;
	float height = m_box.getSize().y;

	//check if click is inside the textbox
	if (CHECKBOUNDS(x, y, pos.x, pos.y, width, height))
	{
		m_selected = true;
		for (int i = m_text.getString().getSize(); i >= 0; --i)
		{
			// Place cursor after the character under the mouse
			sf::Vector2f text = m_text.findCharacterPos(i);
			if (text.x <= x)
			{
				setCursor(i);
				break;
			}
		}
	}
	else
	{
		m_selected = false;
	}
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//draw the text box
	g_window->draw(m_box);
	//draw the text
	g_window->draw(m_text);
	//display a blinking cursor if selected
	if (m_selected)
	{
		if (m_cursor_timer.getElapsedTime().asSeconds() <= CURSORPERIOD)
		{
			g_window->draw(m_cursor);
			m_cursor_timer.restart();
		}
	}
}

void TextBox::setCursor(size_t index)
{
	if (index <= m_string.getSize())
	{
		m_cursor_idx = index;

		m_cursor.setPosition(m_text.findCharacterPos(index).x - m_padding/2, m_text.getPosition().y + m_padding /2);
		m_cursor_timer.restart();
	}
}

void TextBox::setPosition(const sf::Vector2f &pos)
{
	m_position = pos;
	m_box.setPosition(pos);
}
void TextBox::setPosition(float x, float y)
{
	sf::Vector2f pos = sf::Vector2f(x, y);

	m_position = pos;
	m_box.setPosition(pos);
}