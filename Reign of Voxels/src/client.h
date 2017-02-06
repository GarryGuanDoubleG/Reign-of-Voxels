#pragma once
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "SFML/Network.hpp"
#include "EventSystem.h"

#define SERVER_IP_ADDR "192.138.179.239"
#define SERVER_PORT 5000


class Client : public Observer
{
public:
	Client();

	virtual void onNotify(Event event, json obj);
	void SendData(std::string data);
	void onReceiveData(std::string data);
private:
	std::string m_username;
	sf::UdpSocket m_socket;
	sf::Int16 m_port;
};



#endif