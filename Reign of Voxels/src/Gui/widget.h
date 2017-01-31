#pragma once
#include <SFML\Graphics.hpp>

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

	void setID(int id);
	int getID();

	/*
		Position stuff 
	*/
	void setPositiion(const sf::Vector2f &pos);
	void setPosition(float x, float y);
	const sf::Vector2f& getPosition();

	const sf::Vector2f& getSize();

	bool isSelectable();
	bool isSelected();

	virtual void HandleInput(sf::Event event);
	virtual void onMouseEvent(sf::Event event, float x, float y);
	virtual void onTextEnter(char text);
 
	virtual void onTriggerCallback();
protected:
	int m_id;
};