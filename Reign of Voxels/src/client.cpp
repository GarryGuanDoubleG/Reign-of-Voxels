#include <iostream>
#include "client.h"


void Client::SendData()
{
	sf::Packet packet;

	packet << data;
	socket.send(packet, SERVER_IP_ADDR, SERVER_PORT);

	std::cout << username << ": " << data << std::endl;
}

void Client::onReceiveData()
{
	sf::Packet packet;
	
	data = "";
	packet >> data;

	std::cout << "Server" << ": " << data << std::endl;
}