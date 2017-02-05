#include <GL\glew.h>
#include "Gui\Textbox.h"
#include "graphics.h"
#include "simple_logger.h"

#define CURSORPERIOD .75f

TextBox::TextBox()
{
	m_text.setString("");
}

void TextBox::onKeyPressed(sf::Keyboard::Key key)
{
	if (!m_selected)
		return;
	sf::String str = m_text.getString();
	switch (key)
	{
	case sf::Keyboard::Right:
		if (m_cursor_idx < m_text.getString().getSize())
			setCursor(m_cursor_idx + 1);
		break;
	case sf::Keyboard::Left:
		if (m_cursor_idx > 0)
			setCursor(m_cursor_idx - 1);
		break;
	case sf::Keyboard::BackSpace:
		if (m_cursor_idx > 0)
		{
			str.erase(m_cursor_idx - 1);
			setCursor(m_cursor_idx - 1);
		}
		break;
	default:
		break;
	}
	m_text.setString(str);
}

void TextBox::onTextEntered(sf::Uint32 unicode)
{
	if (!m_selected) return;

	if (isalpha(unicode) || isalnum(unicode))
	{
		if (m_text.getString().getSize() < m_max_text_len)
		{
			sf::String str = m_text.getString();
			str.insert(m_cursor_idx,static_cast<char>(unicode));
			m_text.setString(str);
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
	if (index <= m_text.getString().getSize())
	{
		m_cursor_idx = index;

		m_cursor.setPosition(m_text.findCharacterPos(index).x - m_padding.x/4, m_text.getPosition().y + m_padding.y / 2);
		m_cursor_timer.restart();
	}
}

void TextBox::setPosition(const sf::Vector2f &pos)
{
	sf::Vector2f text_pos(pos.x + m_padding.x, pos.y + m_padding.y);
	sf::Vector2f cursor_pos(text_pos.x - m_padding.x / 2, text_pos.y);

	m_box.setPosition(pos);
	m_text.setPosition(text_pos);
	m_cursor.setPosition(cursor_pos);
}
void TextBox::setPosition(float x, float y)
{
	sf::Vector2f pos(x, y);
	sf::Vector2f text_pos(pos.x + m_padding.x, pos.y + m_padding.y);
	sf::Vector2f cursor_pos(text_pos.x - m_padding.x / 2, text_pos.y);

	m_box.setPosition(pos);
	m_text.setPosition(text_pos);
	m_cursor.setPosition(cursor_pos);
}

void TextBox::setTextSize(unsigned int size)
{
	int text_size = size;

	m_text.setCharacterSize(size);
	m_cursor.setSize(sf::Vector2f(text_size / 8, size));
}
void TextBox::setTextColor(sf::Color color)
{
	m_text.setFillColor(color);
	m_cursor.setFillColor(color);
}