#include "image.hpp"
#include "simple_logger.h"
ImageWidget::ImageWidget(const char *filename)
{
	m_sprite = new sf::Sprite();
	m_texture = new sf::Texture();
	if (!m_texture->loadFromFile(filename))
	{
		slog("Could not load image");
	}

	m_texture->setSmooth(true);
	m_sprite->setTexture(*m_texture);
	m_sprite->rotate(90);
	m_sprite->setPosition(-1 * m_texture->getSize().x, -1 * m_texture->getSize().y);
	m_sprite->setColor(sf::Color(255, 255, 255, 200));
}

void ImageWidget::setPosition(const sf::Vector2f &pos)
{
	m_sprite->setPosition(pos);
}
void ImageWidget::setPosition(float x, float y)
{
	m_sprite->setPosition(sf::Vector2f(x, y));
}
void ImageWidget::setSize(float w, float h)
{
	m_sprite->setScale(sf::Vector2f(1.0f/w, 1.0f/h));
}
/**
* @brief Gets the size of the rectangular shape of this widget
* @return 2d vector with width and height
*/
const sf::Vector2f ImageWidget::getSize()
{
	return m_sprite->getScale();
}
void ImageWidget::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	Game::instance().getWindow()->draw(*m_sprite);
}