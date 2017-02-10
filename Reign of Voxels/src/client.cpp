#include <iostream>
#include "client.h"
#include "game.h"
#include "simple_logger.h"

Client::Client(void)
{
	if (enet_initialize() != 0)
		slog("Failed to start enet");

	//client host with 2 channel connection and unthrottled bandwidth
	m_client = enet_host_create(NULL, 1, 2, 0, 0);

	if (m_client == NULL)
		slog("Failed to initiate client");

	m_thread = new sf::Thread(&Client::ConnectionEvent, this);
}

Client::~Client()
{
	Game::instance().getEventSystem().removeObserver(this);
	enet_deinitialize();
	enet_host_destroy(m_client);
}

void Client::onNotify(Event event, Json &obj)
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
	ENetPeer *peer;
	ENetEvent event;
	std::string message;
	int serviceResult = 1;
	Json data;
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, "localhost");
	address.port = SERVER_PORT;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	m_server = enet_host_connect(m_client, &address, 2, 0);
	if (m_server == NULL)
	{
		slog("Could not connect to server");
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		m_connected = true;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(m_server);
	}

	data["type"] = Login;
	data["username"] = m_username;
	data["port"] = m_port;

	SendData(data);
	m_thread->launch();
}

void Client::ConnectionEvent()
{
	ENetEvent event;
	int packs_per_sec = 30;
	int delta_time = 1000 / packs_per_sec;
	
	while (m_connected)
	{
		enet_host_service(m_client, &event, delta_time);

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			ReceiveServerEvent(event);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconected.\n", event.peer->data);
			break;
		}
	}

}

void Client::ReceiveServerEvent(ENetEvent event)
{
	Json data = Json::parse(event.packet->data);
	Event game_event = data["event"];
	
	std::cout << "Data: " << data.dump() << std::endl;
	switch (game_event)
	{
	case JoinLobby:
		Game::instance().getEventSystem().Notify(JoinLobby, data);
		break;
	}

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

void Client::SendData(Json data)
{
	std::string message = data.dump();

	ENetPacket *packet = enet_packet_create(message.c_str(), message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_server, 0, packet);
}


void Client::onReceiveData(std::string data)
{

	//data = "";
	//packet >> data;

	//std::cout << "Server" << ": " << data << std::endl;
}