#pragma once
#include "widget.h"
class Button : public Widget
{
public:
	Button::Button();

	virtual void onMouseEntered(float x, float y);
	
private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};