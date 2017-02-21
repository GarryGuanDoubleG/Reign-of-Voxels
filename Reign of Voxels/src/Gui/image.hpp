#pragma once
#include "widget.hpp"
#include "game.hpp"

class ImageWidget : public Widget
{
public:
	ImageWidget(const char *filename);

	virtual void setPosition(const sf::Vector2f &pos);
	virtual void setPosition(float x, float y);
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
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:
	sf::Sprite *m_sprite;
	sf::Texture *m_texture;
};