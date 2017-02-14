#pragma once
#include "SFML\Graphics.hpp"
#include "layout.h"
#include "textbox.h"
#include "scene.h"
#include "json.hpp"

using Json = nlohmann::json; /**< convenience specified by nlohmann himself */  
/**
* Menu Scene used for login and lobby menus. Manages gui widgets and notifies global events
* when login or lobby menus transition to next scene
*/
class Menu : virtual public Scene
{
public:
	/**
	* A constructor.
	* Specifies to the layout class which type of menu -login or lobby - to load
	* and sets the parent of generated widgets to this menu
	* @param layout the menu layout to load
	*/
	Menu(MenuLayouts layout);
	/**
	* A constructor.
	* Overloaded constructor. Specifies to the layout class which type of menu -login or lobby - to load
	* and updates widget information from given json data. Example is names of players joined in lobby
	* @param layout the menu layout to load
	* @param data json container with widget IDs as the key of the data to update the widgets with
	*/
	Menu(MenuLayouts layout, Json &data);
	/**
	* A destructor.
	* frees the widgets and unsubscribes from event system
	*/
	~Menu();
	/**
	* @brief Clears window background to black and tells widgets to draw themselves to window	
	*/
	virtual void Render();
	/**.
	* @brief Renders menu every scene.
	*/
	virtual void SceneFrame();
	/**.
	* @brief Forwards input to widgets on the screen which may trigger a callback
	* @parma event the input obtained from polling sf events
	*/
	virtual void HandleInput(sf::Event event);
	/**.
	* @brief Callback function triggered by widget. Receives the name of the button event to trigger
	* and collects data from widgets to forward to global event system
	* @param event name of the event
	*/
	virtual void triggerCallBack(sf::String event);		
	/**.
	* @brief callback function from global event system. Updates widget information (new player names from joining lobby) 
	* @param event the event type used to handle data passed
	* @param data the json container which information to update widgets with (join / leaving player names)
	*/
	virtual void onNotify(Event event, Json &data);
	/**.
	* @brief callback function from user input event. forwards the input to HandleInput method
	* @param event tells us it's from server or client
	* @param input the user input
	*/
	virtual void onNotify(Event event, sf::Event &input);
	/**.
	* @brief Loops through each widget and updates string if widget id matches key in passed in data
	* @param data the data with new widget information with keys the same as the widget id.
	*/
	void UpdateWidgets(Json &data);
private:
	std::vector<Widget*> m_widgets;  /**< container for widgets allocated during layout loading */  
	Layout *m_layout; /**< the layout of the menu. referenced to store dynamically loaded types like font */  
};