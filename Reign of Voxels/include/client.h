#pragma once
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "SFML/Network.hpp"

class Client
{
private:
	std::string username;
	sf::UdpSocket udp_sock;
	sf::Int16 sock_port;
public:
	Client(const std::string & name);
	~Client();

};



#endif