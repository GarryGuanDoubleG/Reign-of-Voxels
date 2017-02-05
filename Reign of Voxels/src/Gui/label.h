#pragma once
#include "widget.h"

/*
* @brief Displays text at specified location used alone or alongside
* other widgets as a label
*/

class Label : public Widget
{
public:
	Label();
	void setText(const sf::String &string);
	const sf::String& getText() const;

	void Label::setSubLabel();//set it to small text below assigned widget
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
