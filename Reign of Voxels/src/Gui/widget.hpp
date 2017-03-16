#pragma once
#include <GL/glew.h>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Main.hpp>
#include "glm.hpp"
#include "Json.hpp"
/**
* A macro used to detect 2D collision. Useful to see if mouse click is inside a widget's bounds
*/
#define CHECKBOUNDS(x,y, bx,by,bw,bh) ((x >= bx && y >= by) && (x <= bx + bw && y <= by + bh))
using Json = nlohmann::json; /**< convenience specified by nlohmann himself */  
enum WidgetState
{
	StateIdle, /**<State if widget is not selected  */  
	StateHovered,/**<State if mouse hovers over widget */  
	StateActive/**< State if widget is currently selected */  
};
class Menu; /**<Forward declaring. Widgets Reference a Menu class as parent*/  
class HUD;

/**
* Base class for all gui components that contains sfml text and rectangular shape classes
* Handles user input and triggering callback to parent class
*/
class Widget : public sf::Drawable
{
public:
	/**
	* a constructor
	* empty constructor
	*/
	Widget();
	/**
	* @brief sets the member id string to passed in string
	* @param id the string to set widget id to
	*/
	void setID(std::string id);
	/**
	* @brief returns the id of the widget
	* @return id the id string of this widget
	*/
	std::string getID();
	/**
	* @brief sets the event string sent on gui callback
	* @param event the string name to set event to
	*/
	void setEvent(std::string event);
	/**
	* @brief gets the event string of this widget
	* @param event the string name to set event to
	*/
	sf::String getEvent();
	/**
	* @brief sets the 2d position of the sfml text and rectangularshape class of this widget
	* @param pos the position of the widget to set to
	*/
	virtual void setPosition(const sf::Vector2f &pos);
	/**
	* @brief Overloaded function. Sets the 2d position of the sfml text and rectangularshape class of this widget
	* @param x the x position of the widget to set to
	* @param y the y position of the widget to set to
	*/
	virtual void setPosition(float x, float y);
	/**
	* @brief Gets the 2d position of the rectangularshape class of this widget
	* @return 2d vector float with position of widget
	*/
	virtual sf::Vector2f getPosition();
	/**
	* @brief Sets the widget's parent to passed in menu
	* @param parent the parent that will handle widget input and callback
	*/
	virtual void setParent(Menu * parent);
	/**
	* @brief Sets the size of the rectangular shape of the widget
	* @param w the width of the box
	* @param h the height of the box
	*/
	virtual void setSize(float w, float h);
	/**
	* @brief Gets the size of the rectangular shape of this widget
	* @return 2d vector with width and height
	*/
	const sf::Vector2f getSize();
	/**
	* @brief Sets the padding between the text and the outer rectangular shape	
	* @param padding the 2d vector with x and y padding
	*/
	virtual void setPadding(sf::Vector2f padding);
	/**
	* @brief Sets the font of this widget
	* @param font the font to draw text with
	*/
	virtual void setFont(sf::Font *font);
	/**
	* @brief Sets the text size
	* @param size of the text
	*/
	virtual void setTextSize(unsigned int size);
	/**
	* @brief Sets the text color
	* @param color of the text to set to
	*/
	virtual void setTextColor(sf::Color color);
	/**
	* @brief Sets the box color
	* @param color of the box to set to
	*/
	virtual void setBoxColor(sf::Color color);	
	/**
	* @brief Sets the string of this widget
	* @param color of the text to set to
	*/
	void setString(std::string str);
	/**
	* @brief Sets the max string length of sfml text class
	* @param len the max string length
	*/
	void setMaxStrLen(int len);
	/**
	* @brief Gets the text and id of this widget
	* @return json container with text and id of widget
	*/
	virtual Json getData();
	/**
	* @brief Handles users input. Can update text, do nothing, trigger callback, etc.
	* @param event the user input
	*/
	virtual void HandleInput(sf::Event event);
	/**
	* @brief Handles users text input.
	* @param unicode the unicode value of text entered
	*/
	virtual void onTextEntered(sf::Uint32 unicode);
	/**
	* @brief Handles users keypress.
	* @param key the key the user pressed
	*/
	virtual void onKeyPressed(sf::Keyboard::Key key);
	/**
	* @brief Handles mouse clicks
	* @param x the x position of mouse click
	* @param y the y position of mouse click
	*/
	virtual void onMouseEntered(float x, float y);
	/**
	* @brief Handles mouse movement
	* @param x the x position of mouse movement
	* @param y the y position of mouse movement
	*/
	virtual void onMouseMoved(float x, float y);
	/**
	* @brief Handles state change
	*/
	virtual void onStateChange();
	/**
	* @brief Informs the parent that this widget has triggered a callback
	*/
	virtual void triggerCallBack();
	/*
	* @brief Abstract method. Draws the button box and text into the sfml frame buffer. Inherited from SFML drawables base class.
	* @param target the rendering target, usually window, to draw button on
	* @param states the rendering state to draw with. Usually default rendering states are sent.
	*/
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const = 0;
protected:
	sf::String m_id; /**<  the id of this widget*/  
	sf::String m_event;/**<  name of event to pass to menu upon being triggerd*/  

	bool m_is_selected; /**< flag if widget is selected  */  
	bool m_is_active;/**< flag if widget is active. Used for hiding or showing widgets */  

	sf::Vector2f m_padding; /**< spacing between m_box and m_text */  

	sf::Text m_text; /**< sfml text class that manages font size position and drawing */  
	int m_max_text_len; /**< maximum length m_text is allowed to have */  


	sf::RectangleShape m_box; /**< rectangular shape that stores position and color of widget*/


	Menu * m_parent; /**< menu to alert when callback triggerd*/

};