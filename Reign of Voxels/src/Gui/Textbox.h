#pragma once
#include <GL/glew.h>
#include "Gui\widget.h"

class TextBox : public Widget
{
public: 
	TextBox(sf::Font *font, sf::Vector2f position, float height, float width = 200.0f);

	void setText(const sf::String &string);	
	void setCursor(size_t index);

protected:
	virtual void HandleInput(sf::Event event);
	virtual void Draw(sf::RenderTarget &target, sf::RenderStates states);
private:
	
	sf::String			m_string;
	sf::Text			m_text;
	sf::RectangleShape	m_box;
	sf::RectangleShape	m_cursor;
	sf::Vector2i		m_cursor_box;

	int					m_max_text_length;
	bool				m_selected;
};

