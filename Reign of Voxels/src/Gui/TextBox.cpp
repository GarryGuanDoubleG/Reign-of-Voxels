#include <ctype.h>
#include "SFML\OpenGL.hpp"
#include "Gui\Textbox.h"

TextBox::TextBox(sf::Font *font, float height, float width)
{
	m_box.setSize(sf::Vector2f(width, height));
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

void TextBox::Draw(sf::RenderTarget &target)
{
	//draw the text box
	target.draw(m_box);
	//draw the text
	target.draw(m_text);

	//display a blinking cursor if selected
	if (m_selected)
	{

	}
}