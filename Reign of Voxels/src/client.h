#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <enet\enet.h>
#include <SFML\Graphics.hpp>
#include <SFML/System.hpp>
#include "EventSystem.h"

#define SERVER_IP_ADDR "104.192.170.117"
#define SERVER_PORT 5000

class Client : public Observer
{
public:
	Client();
	~Client();
	virtual void onNotify(Event event, Json &obj);
	virtual void onNotify(Event event, sf::Event &input);
	
	void ConnectHost();//login
	
	void SendData(std::string data);
	void SendData(Json &obj);
	void SendInput(sf::Event event);

	void ConnectionEvent();
	void onReceiveData(ENetEvent event);

	std::string		m_username;
	sf::Uint32		m_port;
private:
	bool			m_game_started;
	bool			m_connected;//connected to server
	sf::Thread		m_thread;
	ENetHost		*m_client;
	ENetPeer		*m_server;
	sf::Clock		m_packet_clock; //time since last packet sent
};

#endif