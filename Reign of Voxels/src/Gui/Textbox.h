#pragma once
#include <GL/glew.h>
#include "Gui\widget.h"

class TextBox : public Widget
{
public: 
	TextBox();

	virtual void setPosition(const sf::Vector2f &pos);
	virtual void setPosition(float x, float y);

	virtual void setTextSize(unsigned int size);
	virtual void setTextColor(sf::Color color);
	void setCursor(size_t index);

	virtual Json getData();

	virtual void onKeyPressed(sf::Keyboard::Key key);
	virtual void onMouseEntered(float x, float y);
	virtual void onTextEntered(sf::Uint32 unicode);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states)const override;
private:
	mutable sf::RectangleShape	m_cursor;
	mutable sf::Clock			m_cursor_timer;
	int					m_cursor_idx;
	bool				m_selected;
};

