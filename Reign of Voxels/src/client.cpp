#include <iostream>
#include "client.hpp"
#include "packet.hpp"
#include "game.hpp"
#include "simple_logger.h"
/**
* constructor
* initializes enet and creates enet host and subscribes to global event system
*/
Client::Client(void) :	m_thread(&Client::ConnectionEvent, this),
						m_game_started(false),
						m_connected(false)
{
	if (enet_initialize() != 0)
		slog("Failed to start enet");

	//client host with 2 channel connection and unthrottled bandwidth
	m_client = enet_host_create(NULL, 1, 2, 0, 0);

	if (m_client == NULL)
		slog("Failed to initiate client");

	Game::instance().getEventSystem().addObserver(this);
}
/**
* Destructor
* unsubscribes from event system and deinitializes enet
*/
Client::~Client()
{
	Game::instance().getEventSystem().removeObserver(this);
	enet_deinitialize();
	enet_host_destroy(m_client);
}
/**
* @brief callback function when game event occurs
* @param event type of event
* @param obj data associated with event. Can be null
*/
void Client::onNotify(Event event, Json &obj)
{
	switch (event)
	{
	case Login:
		if (obj.find("username") != obj.end())
			m_username = obj["username"];
		if (obj.find("port") != obj.end())
			m_port = obj["port"];
		ConnectHost();
		break;
	case Start: 		// game started
	{
		Json data;
		data["event"] = Start;
		SendData(data);
		m_game_started = true;
		break;
	}
	case Close:
		slog("Disconnected");
		if (m_connected)
			enet_peer_disconnect(m_server, 0);
		break;
	default:
		break;
	}
}
/**
* @brief callback function when user inputs
* @param event type of event
* @param input the user input
*/
void Client::onNotify(Event event, sf::Event &input)
{
	switch (event)
	{
	case ClientInput:		
		if (m_connected && m_game_started)
			SendInput(input);
		break;
	}
}
/**
* @brief Connects to server with user and room #
*/
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
	if (!m_connected)
	{
		m_server = enet_host_connect(m_client, &address, 2, 0);
		if (m_server == NULL)
		{
			slog("Could not connect to server");
			return;
		}

		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_connected = true;
			slog("Connected to server");
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(m_server);
		}
	}
	if (m_connected)
	{
		data["event"] = Login;
		data["username"] = m_username;
		data["port"] = m_port;

		SendData(data);
		m_thread.launch();		
	}
}
/**
* @brief Thread launched when logged in. Runs a loop that checks for server packets
*/
void Client::ConnectionEvent()
{
	ENetEvent event;
	int packs_per_sec = 30;
	int delta_time = 1000 / packs_per_sec;
	
	while (m_connected)
	{
		//check if any data received
		enet_host_service(m_client, &event, delta_time);

		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "Connected " << std::endl;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			onReceiveData(event);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconected.\n", event.peer->data);
			m_connected = false;
			break;
		}
	}
}
/**
* @brief manages the data send from the server
* @param event the enet container for packet and packet information
*/
void Client::onReceiveData(ENetEvent event)
{
	char ctype = event.packet->data[0];
	int type = ctype - '0';
	if (type == (int)PacketJson)
	{
		std::cout << "packet in " << (char *)&event.packet->data[1] << std::endl;
		Json data = Json::parse(&event.packet->data[1]);
		Event game_event = data["event"];

		std::cout << "Data: " << data.dump() << std::endl;

		//received data from server
		//have it throw an event and let listeners do whatevs
		//should queue events to avoid loading client thread
		switch (game_event)
		{
		case Error:
			std::cout << "Error: " << data["msg"] << std::endl;
			break;
		case ServerInput:
		{
			/*	std::string temp = data["input"];
				sf::Event new_input = *(sf::Event*)(*(void*)temp.c_str());*/
			break;
		}
		default:
			Game::instance().getEventSystem().Notify(game_event, data);
			break;
		}
	}
	else if (type == (int)PacketSfEvent)
	{
		RoVInput input = *(RoVInput*)&event.packet->data[1];
		Game::instance().getEventSystem().Notify(Event::ServerInput, input.event);
	}
}
/**
* @brief sends a json object to the server
* @param obj the json object to send
*/
void Client::SendData(Json &data)
{
	char type = PacketJson;//rip type safety
	std::string pack = type + data.dump();
	std::cout << "pack " << pack << std::endl;

	ENetPacket *packet = enet_packet_create(pack.c_str(), pack.length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_server, 0, packet);
}
/**
* @brief sends user input to the server
* @param event the user input to send
*/
void Client::SendInput(sf::Event event)
{
	RoVInput my_input;
	my_input.type = (int)PacketSfEvent;
	my_input.event = event;

	ENetPacket *packet = enet_packet_create(&my_input, sizeof(RoVInput), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_server, 0, packet);
}