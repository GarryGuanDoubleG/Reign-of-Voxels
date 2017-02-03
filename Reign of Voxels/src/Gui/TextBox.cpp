#include <GL\glew.h>
#include "Gui\Textbox.h"
#include "graphics.h"
#include "simple_logger.h"

#define uni_isalnum(in) (in >= 0x30 && in < 0x3A)
#define uni_isalpha(in) ((in >= 0x41 && in <= 0x5A) || (in >= 0x61 && in <= 0x7A))

TextBox::TextBox(sf::Font *font, sf::Vector2f position, float height, float width)
{
	int border_size = 2;
	float color[] = { 1.0f, 1.0f, 1.0f };

	m_box.setSize(sf::Vector2f(width, height));
	//border color
	//set position
	m_box.setPosition(position);
	//cursor color
	m_cursor.setFillColor(sf::Color::White);
	m_cursor.setPosition(sf::Vector2f(position.x + (float)border_size, position.y)); //set it to the left of textbox
	
	m_text.setFillColor(sf::Color::Red);
	m_text.setFont(*font);
	m_text.setStyle(sf::Text::Bold);
	m_text.setString("");
	m_text.setPosition(sf::Vector2f(position.x + 5.0f, position.y + 5.0f));
	//string
	m_string.clear();
	m_max_text_length = 16;
}


void TextBox::onKeyPressed(sf::Keyboard::Key key)
{

}

void TextBox::onTextEntered(sf::Uint32 unicode)
{
	if (isalpha(unicode) || isalnum(unicode))
	{
		if (m_string.getSize() < m_max_text_length)
		{
			m_string.insert(m_string.getSize(),static_cast<char>(unicode));
			m_text.setString(m_string);
		}
	}

}

void TextBox::onMouseEntered(float x, float y)
{

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

	}
}