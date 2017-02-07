#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <enet\enet.h>
#include <SFML\Graphics.hpp>
#include "EventSystem.h"

#define SERVER_IP_ADDR "104.192.170.117"
#define SERVER_PORT 5000


class Client : public Observer
{
public:
	Client();
	~Client();
	virtual void onNotify(Event event, json obj);
	void ConnectHost();
	void SendData(std::string data);
	void SendData(json obj);
	void onReceiveData(std::string data);
private:
	ENetHost		*m_client;
	ENetPeer		*m_server;
	std::string		m_username;
	std::string		m_port;
	sf::Clock		m_packet_clock; //time since last packet sent
};



#endif