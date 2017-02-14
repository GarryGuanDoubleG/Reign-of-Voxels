#include "widget.hpp"
#include "game.hpp"
#include "menu.hpp"
/**
* a constructor
* empty constructor
*/
Widget::Widget()
{

}
/**
* @brief sets the member id string to passed in string
* @param id the string to set widget id to
*/
void Widget::setID(std::string id)
{
	m_id = id;
}
/**
* @brief returns the id of the widget
* @return id the id string of this widget
*/
std::string Widget::getID()
{
	return m_id;
}
/**
* @brief sets the event string sent on gui callback
* @param event the string name to set event to
*/
void Widget::setEvent(std::string event)
{
	m_event = event;
}
/**
* @brief gets the event string of this widget
* @param event the string name to set event to
*/
sf::String Widget::getEvent()
{
	return m_event;
}
/**
* @brief sets the 2d position of the sfml text and rectangularshape class of this widget
* @param pos the position of the widget to set to
*/
void Widget::setPosition(const sf::Vector2f &pos)
{
	m_box.setPosition(pos);
	m_text.setPosition(sf::Vector2f(pos.x + m_padding.x, pos.y + m_padding.y));
}
/**
* @brief Overloaded function. Sets the 2d position of the sfml text and rectangularshape class of this widget
* @param x the x position of the widget to set to
* @param y the y position of the widget to set to
*/
void Widget::setPosition(float x, float y)
{
	sf::Vector2f pos(x, y);
	m_box.setPosition(pos);
	m_text.setPosition(sf::Vector2f(pos.x + m_padding.x, pos.y + m_padding.y));
}
/**
* @brief Gets the 2d position of the rectangularshape class of this widget
* @return 2d vector float with position of widget
*/
sf::Vector2f Widget::getPosition()
{
	return m_box.getPosition();
}
/**
* @brief Sets the widget's parent to passed in menu
* @param parent the parent that will handle widget input and callback
*/
void Widget::setParent(Menu * parent)
{
	this->m_parent = parent;
}
/**
* @brief Sets the size of the rectangular shape of the widget
* @param w the width of the box
* @param h the height of the box
*/
void Widget::setSize(float w, float h)
{
	m_box.setSize(sf::Vector2f(w, h));
}
/**
* @brief Gets the size of the rectangular shape of this widget
* @return 2d vector with width and height
*/
const sf::Vector2f Widget::getSize()
{
	return m_box.getSize();
}
/**
* @brief Sets the font of this widget
* @param font the font to draw text with
*/
void Widget::setFont(sf::Font *font)
{
	m_text.setFont(*font);
}
/**
* @brief Sets the text color
* @param color of the text to set to
*/
void Widget::setTextColor(sf::Color color)
{
	m_text.setFillColor(color);
}
/**
* @brief Sets the text size
* @param size of the text
*/
void Widget::setTextSize(unsigned int size)
{
	m_text.setCharacterSize(size);
}
/**
* @brief Sets the padding between the text and the outer rectangular shape
* @param padding the 2d vector with x and y padding
*/
void Widget::setPadding(sf::Vector2f padding)
{
	m_padding = padding;
}
/**
* @brief Sets the string of this widget
* @param color of the text to set to
*/
void Widget::setString(std::string str)
{
	std::string string = str;
	std::cout << "label " << string << std::endl;
	m_text.setString(str);
}
/**
* @brief Sets the max string length of sfml text class
* @param len the max string length
*/
void Widget::setMaxStrLen(int len)
{
	m_max_text_len = len;
}
/**
* @brief Sets the box color
* @param color of the box to set to
*/
void Widget::setBoxColor(sf::Color color)
{
	m_box.setFillColor(color);
}
/**
* @brief Handles users input. Can update text, do nothing, trigger callback, etc.
* @param event the user input
*/
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
		onMouseEntered(sf::Mouse::getPosition(*Game::instance().getWindow()).x, sf::Mouse::getPosition(*Game::instance().getWindow()).y);
		break;
	}
	default:
		break;
	}
}
/*
* @brief Abstract method. Draws the button box and text into the sfml frame buffer. Inherited from SFML drawables base class.
* @param target the rendering target, usually window, to draw button on
* @param states the rendering state to draw with. Usually default rendering states are sent.
*/
void Widget::draw(sf::RenderTarget &target, sf::RenderStates states) const
{

}
/**
* @brief Handles users keypress.
* @param key the key the user pressed
*/
void Widget::onKeyPressed(sf::Keyboard::Key key)
{

}
/**
* @brief Handles users text input.
* @param unicode the unicode value of text entered
*/
void Widget::onTextEntered(sf::Uint32 unicode)
{

}
/**
* @brief Handles mouse clicks
* @param x the x position of mouse click
* @param y the y position of mouse click
*/
void Widget::onMouseEntered(float x, float y)
{

}
/**
* @brief Handles mouse movement
* @param x the x position of mouse movement
* @param y the y position of mouse movement
*/
void Widget::onMouseMoved(float x, float y)
{

}
/**
* @brief Handles state change
*/
void Widget::onStateChange()
{

}
/**
* @brief Informs the parent that this widget has triggered a callback
*/
void Widget::triggerCallBack()
{
	m_parent->triggerCallBack(this->m_event);
}
/**
* @brief Gets the text and id of this widget
* @return json container with text and id of widget
*/
Json Widget::getData()
{
	Json data = nullptr;
	return data;
}