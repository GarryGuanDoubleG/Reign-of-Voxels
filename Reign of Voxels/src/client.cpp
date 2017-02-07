#include <iostream>
#include "SFML\Network.hpp"
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
		SendData("test");
	}

}

void Client::SendData(std::string data)
{
	////////////////////////////////////////////////////////////
	/// Send a message to the server, wait for the answer
	///
	////////////////////////////////////////////////////////////

	// Ask for the server address
	sf::IpAddress server = SERVER_IP_ADDR;

	// Create a socket for communicating with the server
	sf::UdpSocket socket;

	// Send a message to the server
	const char out[] = "Hi, I'm a client";
	if (socket.send(out, sizeof(out), server, 5000) != sf::Socket::Done)
		return;
	std::cout << "Message sent to the server: \"" << out << "\"" << std::endl;

	// Receive an answer from anyone (but most likely from the server)
	char in[128];
	std::size_t received;
	sf::IpAddress sender;
	unsigned short senderPort;
	if (socket.receive(in, sizeof(in), received, sender, senderPort) != sf::Socket::Done)
		return;
	std::cout << "Message received from " << sender << ": \"" << in << "\"" << std::endl;
}

void Client::onReceiveData(std::string data)
{
	sf::Packet packet;
	
	//data = "";
	//packet >> data;

	//std::cout << "Server" << ": " << data << std::endl;
}