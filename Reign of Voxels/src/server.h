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
	Server();
	~Server();

	void SendData();
	void onReceiveData();
	void ServerLoop();
};
#endif