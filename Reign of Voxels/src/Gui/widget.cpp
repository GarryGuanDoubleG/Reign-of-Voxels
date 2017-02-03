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

}

void Widget::Draw(sf::RenderTarget &target, sf::RenderStates states)
{

}