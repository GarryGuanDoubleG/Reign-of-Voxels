#pragma once
#include <GL/glew.h>
#include "Gui\widget.h"
/**
* A textbox widget. Handles text input and max character limit
*/
class TextBox : public Widget
{
public: 

	//! A constructor.
	/*!
	Constructor for a textbox used for the menu scene
	Sets inner text to null
	*/
	TextBox();
	/*
	* @brief Sets the position of the sfml text - with padding - and sfml rectangular shape to specified position
	* @param pos the postion to set the textbox to
	*/
	virtual void setPosition(const sf::Vector2f &pos);
	/*
	* @brief Sets the position of the sfml text - with padding - and sfml rectangular shape to specified position
	* @param x the x postion to set the textbox to
	* @param y the y postion to set the textbox to
	*/
	virtual void setPosition(float x, float y);
	/*
	* @brief Sets the textsize of the textbox
	* @param size the size of the text to set to
	*/
	virtual void setTextSize(unsigned int size);
	/*
	* @brief Sets the text color of the textbox
	* @param color the color to set the text to
	*/
	virtual void setTextColor(sf::Color color);
	/*
	* @brief Sets the cursor position in front of a character
	* @param index the index of the character in the text to move in front of
	*/
	void setCursor(size_t index);
	/*
	* @brief Gets the string currently entered in the textbox
	* @return a json container with the id and string of this textbox
	*/
	virtual Json getData();
	/*
	* @brief Handles keypress event. Mainly left & right arrow and backspace key presses update text
	* @param the key pressed
	*/
	virtual void onKeyPressed(sf::Keyboard::Key key);
	/*
	* @brief Checks if mouse click is in widget bounds and places cursor on character clicked
	* @param x the x coordinate of mouse click
	* @param y the y coordinate of mouse click
	*/
	virtual void onMouseEntered(float x, float y);

	/*
	* @brief Updates text of the textbox if character is a number or letter
	* @param unicode the unicode value of the text entered
	*/
	virtual void onTextEntered(sf::Uint32 unicode);
private:
	/*
	* @brief Draws the textbox box and text into the sfml frame buffer. Inherited from SFML drawables base class.
	* @param target the rendering target, usually window, to draw button on
	* @param states the rendering state to draw with. Usually default rendering states are sent.
	*/
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states)const override;

	mutable sf::RectangleShape	m_cursor; /**< shape and color container for cursor */  
	mutable sf::Clock			m_cursor_timer; /**< the cursor blink timer*/  
	int					m_cursor_idx; /**<the index of the text the cursor is positioned in front of */  
	bool				m_selected; /**<flag for whether textbox is the focused gui  */  
};

