#pragma once
#include "game.h"
#include "Json.hpp"
#include "widget.h"
#include "scene.h"

using Json = nlohmann::json; /**< convenience specified by nlohmann himself */  
enum MenuLayouts
{
	LoginMenu, /**< enum value LoginMenu indicating login menu scene or layout should be used */  
	LobbyMenu, /**< enum value LobbyMenu indicating Lobby menu scene or layout should be used */  
	OptionsMenu /**< enum value OptionsMenu indicating Options menu scene or layout should be used */  
};
/**
* A utility class used to load gui position, color, text, events, and size from a JSON file
*/
class Layout
{
public:
	/*
	* @brief Reads a specific menu layout from a json file and generates widgets from data outlined for each component
	* @param menu the layout to load
	* @return a vector container of all the widgets generated. used for getting data from gui components
	*/
	std::vector<Widget*> LoadMenuLayout(MenuLayouts menu);
	/*
	* @brief Loads widget position, size, textcolor, boxcolor, textsize, box size, trigger event, and id
	  into newly allocated widget.
	* @param data the json container of the specific widget being loaded
	* @param widget the newly allocated widget.
	*/
	void LoadWidgetData(Json &data, Widget *widget);
	/*
	* @brief Returns the font being used by this layout
	* @return the font this layout is using
	*/
	sf::Font &getFont();	
	/*
	* @brief Returns the padding being used by this layout
	* @return the padding this layout is using
	*/
	sf::Vector2f getPadding();
	//could add background image and other gui stuff too
protected:
	sf::Font		*m_font; /**< font for all widgets in layout */  
	sf::Vector2f	m_padding; /**< padding for all widgets in layout*/  
};


