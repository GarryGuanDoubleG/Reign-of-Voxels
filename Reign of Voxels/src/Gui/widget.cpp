#include "widget.h"
#include "graphics.h"
#include "menu.h"

Widget::Widget()
{

}

void Widget::setID(sf::String id)
{
	m_id = id;
}

sf::String Widget::getID()
{
	return m_id;
}

void Widget::setEvent(std::string event)
{
	m_event = event;
}

sf::String Widget::getEvent(std::string event)
{
	return m_event;
}

void Widget::setPosition(const sf::Vector2f &pos)
{
	m_box.setPosition(pos);
	m_text.setPosition(sf::Vector2f(pos.x + m_padding.x, pos.y + m_padding.y));
}
void Widget::setPosition(float x, float y)
{
	sf::Vector2f pos(x, y);
	m_box.setPosition(pos);
	m_text.setPosition(sf::Vector2f(pos.x + m_padding.x, pos.y + m_padding.y));
}

sf::Vector2f Widget::getPosition()
{
	return m_box.getPosition();
}
void Widget::setParent(Menu * parent)
{
	this->m_parent = parent;
}

void Widget::setSize(float w, float h)
{
	m_box.setSize(sf::Vector2f(w, h));
}

const sf::Vector2f Widget::getSize()
{
	return m_box.getSize();
}

void Widget::setFont(sf::Font *font)
{
	m_text.setFont(*font);
}

void Widget::setTextColor(sf::Color color)
{
	m_text.setFillColor(color);
}
void Widget::setTextSize(unsigned int size)
{
	m_text.setCharacterSize(size);
}
void Widget::setPadding(sf::Vector2f padding)
{
	m_padding = padding;
}
void Widget::setString(std::string str)
{
	std::string string = str;
	std::cout << "label " << string << std::endl;
	m_text.setString(str);
}
void Widget::setMaxStrLen(int len)
{
	m_max_text_len = len;
}
void Widget::setBoxColor(sf::Color color)
{
	m_box.setFillColor(color);
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
	{
		onMouseEntered(sf::Mouse::getPosition(*g_window).x, sf::Mouse::getPosition(*g_window).y);
		break;
	}
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

void Widget::onMouseMoved(float x, float y)
{

}

void Widget::onStateChange()
{

}

void Widget::triggerCallBack()
{
	m_parent->triggerCallBack(this->m_event);
}

Json Widget::getData()
{
	Json data = nullptr;
	return data;
}