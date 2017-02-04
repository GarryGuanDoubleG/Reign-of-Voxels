#pragma once
#include <GL/glew.h>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Main.hpp>
enum WidgetState
{
	StateIdle,
	StateHovered,
	StateActive
};

#define CHECKBOUNDS(x,y, bx,by,bw,bh) ((x >= bx && y >= by) && (x <= bx + bw && y <= by + bh))

class Widget : public sf::Drawable
{
public:
	Widget();

	void setID(int id);
	int getID();
	/*
		Position stuff 
	*/
	virtual void setPosition(const sf::Vector2f &pos);
	virtual void setPosition(float x, float y);
	virtual sf::Vector2f getPosition();
	const sf::Vector2f getSize();

	bool isSelectable();
	bool isSelected();

	virtual void HandleInput(sf::Event event);
	virtual void onTextEntered(sf::Uint32 unicode);
	virtual void onKeyPressed(sf::Keyboard::Key key);
	virtual void onMouseEntered(float x, float y);
	virtual void onMouseMoved(float x, float y);
	virtual void onStateChange();
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
protected:
	int m_id;
	bool m_is_selected;

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	
	sf::Text m_text;


};