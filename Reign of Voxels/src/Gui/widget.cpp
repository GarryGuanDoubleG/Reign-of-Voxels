#include "widget.h"

Widget::Widget()
{

}

void Widget::setID(int id)
{
	m_id = id;
}

int Widget::getID()
{
	return m_id;
}
void Widget::setPositiion(const sf::Vector2f &pos)
{
	m_position = pos;
}
void Widget::setPosition(float x, float y)
{
	m_position = sf::Vector2f(x, y);
}

sf::Vector2f Widget::getPosition()
{
	return m_position;
}

const sf::Vector2f Widget::getSize()
{
	return m_size;
}

void Widget::HandleInput(sf::Event event)
{
	//handle keyboard inputs
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		onKeyPressed(event.key.code);
		break;
	case sf::Event::TextEntered:
		if (event.text.unicode < 128)
			onTextEntered(event.text.unicode);
		break;
	case sf::Event::MouseButtonPressed:
		break;
	default:
		break;
	}
}

void Widget::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

}

void Widget::onKeyPressed(sf::Keyboard::Key key)
{

}

void Widget::onTextEntered(sf::Uint32 unicode)
{

}

void Widget::onMouseEntered(float x, float y)
{

}