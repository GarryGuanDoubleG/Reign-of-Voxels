#include <iostream>
#include "server.h"

void Server::SendData()
{
	sf::Packet packet;
	std::string data = "I got your response";
	sf::IpAddress receive_ip = "192.138.179.239";
	unsigned short port = 2000;

	packet << data;
	socket.send(packet, receive_ip, port);

	std::cout << "Server" << ": " << data << std::endl;
}

void Server::onReceiveData()
{
	sf::Packet packet;

	data = "";
	packet >> data;

	
	std::cout << username << ": " << data << std::endl;
}

void Server::ServerLoop()
{
	sf::Packet receive_packet;
	while (true)
	{
		sf::IpAddress senderIp;//receiving ip
		unsigned short senderPort;
		
		socket.receive(receive_packet, senderIp, senderPort);
	}
}