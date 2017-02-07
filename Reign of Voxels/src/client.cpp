#include <iostream>
#include "client.h"
#include "game.h"
#include "simple_logger.h"

Client::Client()
{
	if (enet_initialize() != 0)
		slog("Failed to start enet");

	m_client = enet_host_create(NULL,
		1,
		2,
		0,
		0);

	if (m_client == NULL)
		slog("Failed to initiate client");

	ConnectHost();
	Game::instance().getEventSystem().addObserver(this);
}

Client::~Client()
{
	Game::instance().getEventSystem().removeObserver(this);
	enet_deinitialize();
	enet_host_destroy(m_client);
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
		ConnectHost();
	}

}

void Client::ConnectHost()
{
	ENetAddress address;
	ENetEvent event;
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, SERVER_IP_ADDR);
	address.port = SERVER_PORT;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	m_server = enet_host_connect(m_client, &address, 2, 0);
	if (m_server == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Garry Guan has succeeded.");
		slog("Connected to Host: success");
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(m_server);
	}
	std::string test = "test";
	SendData(test);
}

void Client::SendData(std::string data)
{
	/* Create a reliable packet of size 7 containing "packet\0" */
	ENetPacket * packet = enet_packet_create(data.c_str(),
		strlen("packet") + 1,
		ENET_PACKET_FLAG_RELIABLE);
	/* Extend the packet so and append the string "foo", so it now */
	/* contains "packetfoo\0"                                      */
	
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_peer_send(m_server, 0, packet);

}

void Client::SendData(json data)
{

}


void Client::onReceiveData(std::string data)
{

	//data = "";
	//packet >> data;

	//std::cout << "Server" << ": " << data << std::endl;
}