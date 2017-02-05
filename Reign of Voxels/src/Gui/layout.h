#pragma once
#include "graphics.h"
#include "json.hpp"
#include "widget.h"

using json = nlohmann::json;

enum MenuLayouts
{
	Login,
	Lobby,
	GameOptions
};

class Layout
{
public:
	std::vector<Widget*> LoadMenuLayout(MenuLayouts menu);
	void LoadWidgetData(json data, Widget *widget);
	sf::Font &getFont();

	sf::Vector2f getBoxSize();
	sf::Vector2f getPadding();
	sf::Vector2f getMargin();
	int getTextSize();
	//could add background image and other gui stuff too
protected:
	sf::Font		*m_font;
	sf::Vector2f	m_padding,
					m_margin;

	bool LoadFont(const std::string &path);
};


