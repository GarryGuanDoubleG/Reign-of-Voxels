#include <iostream>
#include "client.h"
#include "game.h"

Client::Client()
{
	Game::instance().getEventSystem().addObserver(this);
}


void Client::onNotify(Event event, json obj)
{
	if (event == Event::Login)
	{
		if (obj.find("username") != obj.end())
		{
			std::string username = obj["username"];
			m_username = username;
		}
		if (obj.find("port") != obj.end())
		{
			std::string port = obj["port"];
			m_port = std::stoi(port);
		}
	}

}

void Client::SendData(std::string data)
{
	sf::Packet packet;

	packet << data;
	//socket.send(packet, SERVER_IP_ADDR, SERVER_PORT);

	//std::cout << username << ": " << data << std::endl;
}

void Client::onReceiveData(std::string data)
{
	sf::Packet packet;
	
	//data = "";
	//packet >> data;

	//std::cout << "Server" << ": " << data << std::endl;
}