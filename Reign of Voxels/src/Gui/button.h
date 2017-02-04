#pragma once
#include "widget.h"
class Button : public Widget
{
public:
	Button(sf::Font * font, sf::String &string, sf::Vector2f position, float width, float height);

	virtual void setString(const sf::String& string);

	virtual void HandleInput();
	virtual void onKeyPressed(sf::Keyboard::Key key);
	virtual void onMouseEntered(float x, float y);
	virtual void onMouseMoved(float x, float y);
private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	sf::RectangleShape m_box;
};