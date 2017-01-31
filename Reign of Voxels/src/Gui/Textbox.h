#pragma once
#include <SFML\Graphics.hpp>

class TextBox
{
public: 
	TextBox(sf::Font *font, float height, float width = 200.0f);

	void setText(const sf::String &string);	
	void setCursor(size_t index);

protected:
	void HandleInput(sf::Event event);
private:
	void Draw(sf::RenderTarget &target);
	
	sf::String			m_string;
	sf::Text			m_text;
	sf::RectangleShape	m_box;
	sf::RectangleShape	m_cursor;
	sf::Vector2i		m_cursor_box;
	int					m_max_text_length;
	bool				m_selected;
};

