#pragma once
#include "widget.h"

/*
* @brief Displays text at specified location used alone or alongside
* other widgets as a label
*/

class Label : public Widget
{
public:
	Label(sf::Font * font, Widget *widget, const sf::String& string);
	void setText(const sf::String &string);
	const sf::String& getText() const;

	void setColor(const sf::Color &color);
	const sf::Color& getColor() const;

	void setTextSize(size_t size);
	size_t getTextSize() const;
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	sf::Text m_text;
};