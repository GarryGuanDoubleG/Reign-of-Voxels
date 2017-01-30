#pragma once
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "SFML/Network.hpp"

#define SERVER_IP_ADDR "192.138.179.239"
#define SERVER_PORT 5000

class Client
{
private:
	std::string username;
	sf::UdpSocket socket;
	sf::Int16 port;
	std::string data;
public:
	Client(const std::string & name)
	{
		username = name;
		port = rand() % 5000;
		if (socket.bind(port) != sf::Socket::Done)
		{
			printf("Error binding \n");
		}
	}
	~Client();

	void SendData();
	void onReceiveData();

};



#endif