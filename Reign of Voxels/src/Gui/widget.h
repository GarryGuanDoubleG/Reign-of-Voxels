#pragma once
#include <GL/glew.h>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Main.hpp>

#define CHECKBOUNDS(x,y, bx,by,bw,bh) ((x >= bx && y >= by) && (x <= bx + bw && y <= by + bh))

enum WidgetState
{
	StateIdle,
	StateHovered,
	StateActive
};

class Widget : public sf::Drawable
{
public:
	Widget();

	void setID(sf::String id);
	sf::String getID();
	/*
		Position stuff 
	*/
	virtual void setPosition(const sf::Vector2f &pos);
	virtual void setPosition(float x, float y);
	virtual sf::Vector2f getPosition();
	//set box size
	virtual void setSize(float w, float h);
	const sf::Vector2f getSize();
	//padding
	virtual void setPadding(sf::Vector2f padding);
	//font
	virtual void setFont(sf::Font *font);
	//text
	virtual void setTextSize(unsigned int size);
	virtual void setTextColor(sf::Color color);
	virtual void setBoxColor(sf::Color color);
	//display text
	void setString(sf::String str);
	void setMaxStrLen(int len);
	bool isSelectable();
	bool isSelected();
	
	//callbacks
	virtual void HandleInput(sf::Event event);
	virtual void onTextEntered(sf::Uint32 unicode);
	virtual void onKeyPressed(sf::Keyboard::Key key);
	virtual void onMouseEntered(float x, float y);
	virtual void onMouseMoved(float x, float y);
	virtual void onStateChange();

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
protected:
	sf::String m_id;
	bool m_is_selected;

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	sf::Vector2f m_padding;

	bool m_is_active;//draw if active

	sf::Text m_text;
	int m_max_text_len;

	sf::RectangleShape m_box;
};