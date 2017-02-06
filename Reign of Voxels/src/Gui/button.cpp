#include "button.h"

Button::Button()
{
	m_text.setString("");
}


void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(m_box);
	target.draw(m_text);
}

void Button::onMouseEntered(float x, float y)
{
	sf::Vector2f pos = m_box.getPosition();
	float width = m_box.getSize().x;
	float height = m_box.getSize().y;

	//if button click, trigger callbacks
	if (CHECKBOUNDS(x, y, pos.x, pos.y, width, height))
	{
		triggerCallBack();
	}

}

