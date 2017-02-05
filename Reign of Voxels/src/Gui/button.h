#pragma once
#include "widget.h"
class Button : public Widget
{
public:
	Button::Button(sf::Font * font, const sf::String &string, sf::Vector2f position, float width, float height);

	virtual void setPosition(sf::Vector2f position);

	virtual void setString(const sf::String& string);
	virtual void onMouseEntered(float x, float y);
	
private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	sf::RectangleShape m_box;
};