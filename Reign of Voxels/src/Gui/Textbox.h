#pragma once
#include <GL/glew.h>
#include "Gui\widget.h"

class TextBox : public Widget
{
public: 
	TextBox(sf::Font *font, sf::Vector2f position, float height, float width = 200.0f);

	virtual void setPosition(const sf::Vector2f &pos);
	virtual void setPosition(float x, float y);

	void setCursor(size_t index);

	virtual void onKeyPressed(sf::Keyboard::Key key);
	virtual void onMouseEntered(float x, float y);
	virtual void onTextEntered(sf::Uint32 unicode);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states)const override;
private:
	
	sf::String			m_string;
	sf::RectangleShape	m_box;

	mutable sf::RectangleShape	m_cursor;
	mutable sf::Clock			m_cursor_timer;
	int					m_cursor_idx;

	int					m_padding;
	int					m_max_text_length;
	bool				m_selected;
};

