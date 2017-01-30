#pragma once
#include <SFML\Graphics.hpp>

class TextBox
{
public: 
	TextBox(float width = 200.f);

	void setText(const sf::String &string);	
	void setCursor(size_t index);

protected:
	void HandleInput(sf::Event event);
private:
	void Draw(sf::RenderTarget &target, sf::RenderStates states);
	
	sf::String			m_string;
	sf::Text			m_text;
	sf::RectangleShape	m_box;
	sf::RectangleShape	m_cursor;
	sf::Vector2i		m_cursor_box;
	int					m_length;
	bool				m_selected;
};

