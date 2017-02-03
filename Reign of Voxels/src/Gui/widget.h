#pragma once
#include <GL/glew.h>
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
	sf::Vector2f getPosition();
	const sf::Vector2f getSize();

	bool isSelectable();
	bool isSelected();

	virtual void HandleInput(sf::Event event);
	virtual void Draw(sf::RenderTarget &target, sf::RenderStates states);
protected:
	int m_id;
	bool m_is_selected;

	sf::Vector2f m_position;
	sf::Vector2f m_size;
};