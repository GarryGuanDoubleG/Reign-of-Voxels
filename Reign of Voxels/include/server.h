#pragma once
#ifndef _SERVER_H
#define _SERVER_H
#include "SFML/Network.hpp"

#define MAX_CLIENTS 100;

class Server
{
private:
	std::string username;
	sf::UdpSocket socket;
	sf::Int16 port;
	std::string data;
public:
	Server(const std::string & name)
	{
		username = name;
		if (socket.bind(port) != sf::Socket::Done)
		{
			printf("Error binding \n");
		}
	}
	~Server();

	void SendData();
	void onReceiveData();
	void ServerLoop();
};
#endif