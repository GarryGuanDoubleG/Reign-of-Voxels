#include <ctype.h>
#include <GL\glew.h>
#include "SFML\OpenGL.hpp"
#include "Gui\Textbox.h"
#include "graphics.h"

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
	
	m_text.setFillColor(sf::Color::White);
	m_text.setFont(*font);
}

void TextBox::HandleInput(sf::Event event)
{
	int input = event.text.unicode;

	//handle keyboard inputs
	switch (event.key.code)
	{
	case sf::Keyboard::BackSpace:
		//erase last character
		if (m_string.getSize() > 0)
			m_string.erase(m_string.getSize() - 1, m_string.getSize());
		break;
	case sf::Event::TextEntered:
		//add character to textbox string
		if (m_string.getSize() < m_max_text_length)
		{
			if ((iswalpha(input)) || (iswalnum(input)))
			{
				m_string += (char)event.text.unicode;
			}
		}
		break;
 	default:
		break;
	}
	m_text.setString(m_string);
}

void TextBox::Draw(sf::RenderTarget &target, sf::RenderStates states)
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