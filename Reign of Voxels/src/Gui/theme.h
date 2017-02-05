#pragma once
#include <SFML\Graphics.hpp>
#include "widget.h"
#include "textbox.h"
#include "button.h"
#include "label.h"
enum MenuThemes
{
	Login,
	Lobby,
	GameOptions
};

class Theme
{
public:
	static Theme &instance(MenuThemes theme);

	static void LoadMenuTheme(MenuThemes theme);
	sf::Font &getFont();

	sf::Vector2f getBoxSize();
	sf::Vector2f getPadding();
	sf::Vector2f getMargin();
	int getTextSize();
	//could add background image and other gui stuff too
protected:
	sf::Font m_font;
	sf::Texture m_texture;

	virtual bool LoadFont(const std::string &path);
	virtual void LoadWidgets();
};


class LoginTheme :public Theme
{
public:
	LoginTheme();
private:

	
};
