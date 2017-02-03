#include "Menu.h"
#include "graphics.h"
#include "label.h"
Menu::Menu()
{
	m_font = new sf::Font();

	if (!m_font->loadFromFile("Resources/fonts/Quicksand-Regular.otf"))
		printf("Could not load font %s\n", "Resources/fonts/Quicksand-Regular.otf");

	sf::Vector2f win_size = GraphicsGetWinSizef();
	
	state_ = STATE_IDLE;
	
	TextBox * login_box = new TextBox(m_font, sf::Vector2f(win_size.x/2.0 - 150, win_size.y/2.00f - 50.0f), 50, 300);
	Label * login_label = new Label(m_font, login_box, "Username");

	TextBox * port = new TextBox(m_font, sf::Vector2f(win_size.x / 2.0 - 150, win_size.y / 2.00f + 50.0f), 50, 300);
	Label * port_label = new Label(m_font, port, "Port#");

	m_widgets.push_back(login_box);
	m_widgets.push_back(login_label);
	m_widgets.push_back(port);
	m_widgets.push_back(port_label);


}

//handle menu loop
//will later extend class for different types of menus
//doing login first to set up for network
void Menu::MenuLoop()
{
	while (g_window->isOpen())
	{
		sf::Event event;
		while (g_window->pollEvent(event))
		{
			HandleInput(event);
		}	
		Render();
	}
}

//render the menu to the screen
//will cover the entire window
void Menu::Render()
{
	//set background to black
	g_window->clear(sf::Color::Blue);
	//save gl states
	g_window->pushGLStates();
	//draw text
	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->draw(*g_window, sf::RenderStates::Default);
	}
	//restore the gl states
	g_window->popGLStates();
	g_window->display();
}

void Menu::HandleInput(sf::Event event)
{
	if ((event.type == sf::Event::Closed) || event.key.code == sf::Keyboard::Escape)
		g_window->close();

	for (int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->HandleInput(event);
	}

	g_window->display();
}