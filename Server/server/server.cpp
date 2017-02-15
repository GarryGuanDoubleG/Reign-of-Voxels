#include <enet/enet.h>
#include <map>
#include <iostream>
#include "packet.h"

ENetAddress g_address; /**< enet address of server to create*/
ENetHost * g_server; /**< enet host for clients to connect to*/

#define SERVER_IP "rov.garryguan.com" /**<server host address to connect to */
#define LOGIN_PORT 5000 /**<port to send packets through over UDP */

#define MAX_CLIENTS 400 /**<max players sending packets to server at the same time*/
#define MAX_INSTANCES 100 /**<max instances server can handle */
#define MAX_PLAYERS_PER_INSTANCE 4 /**<max number of clients per instance*/

/**
* Game instance struct that stores information about each game instance
*/
typedef struct
{
	bool is_running; /**< flag for game instance initiated*/
	ENetPeer		**players;/**< array of pointers to players in instance  */
	Json			data[MAX_PLAYERS_PER_INSTANCE];/**<stores information about each player in instance*/
}GameInstance;

/**
* \Remarks This is tightly coupled with client events. Anytime events in client side is updated, this should update to exactly match it
*/
enum Event
{
	Error, /**< Server has encountered an error */
	Login, /**< Player starts to login */
	JoinLobby, /**< Login succeeds and joins a lobby */
	JoinPlayer, /**< Another player joins same lobby  */
	LeavePlayer, /**< Someone in lobby left */
	ClientInput, /**< Player input locally */
	ServerInput,/**<  Player input sent from server*/
	Chat, /**<  Player typed something in chat*/
	Start, /**< Player starts the game*/
	InitGame, /**< Server response to inform all players to initiate game*/
	Close /**< Close the game and start cleaning up*/
};

std::map<ENetPeer *, GameInstance *> g_player_instance_map; /**<map of game instances with client as the key*/
GameInstance *g_instances; /**<list of game instances*/
/**
* @brief Overloaded function. Sets first byte of packet to appropriate packet type.
* Creates packet and sends it to player
* @param player to send packet to
* @param data the json data to send
*/
void SendData(ENetPeer *player, Json &data)
{
	std::cout << "player port is " << player->address.port << std::endl;

	int type = PacketJson;
	std::string packet = std::to_string(type) + data.dump();

	std::cout << "packet out " << packet << std::endl;

	ENetPacket *enet_packet = enet_packet_create(packet.c_str(), packet.length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(player, 1, enet_packet);
	//enet_packet_destroy(packet);
}

/**
* @brief Overloaded function. Sets first byte of packet to appropriate packet type. 
* Creates packet and sends it to player
* @param player to send packet to
* @param data the user input to send
*/
void SendData(ENetPeer *player, RoVInput &data)
{
	char buffer[sizeof(RoVInput) + 1];
	buffer[0] = '2';
	memcpy(buffer + 1, &data, sizeof(RoVInput));

	ENetPacket *enet_packet = enet_packet_create(buffer, sizeof(buffer), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(player, 1, enet_packet);
}
/**
* @brief Sends a packet with an event and message to a player
* @param player the player to send the message to
* @param message the message to send
* @param event the event associated with message
*/
void SendMessage(ENetPeer *player, std::string message, Event event)
{
	Json response;
	response["event"] = event;
	response["msg"] = message;

	SendData(player, response);
}
/**
* @brief Sends json data to everyone in an instance except the client that triggered event to send data
* @param instance the instance to send data to
* @param sender the client in instance not to send to
* @param data the packet data to send
*/
void UpdateOthers(GameInstance *instance, ENetPeer *sender, Json &data)
{
	//update players already in lobby
	for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
	{
		if (instance->players[i] && (instance->players[i] != sender))
		{
			SendData(instance->players[i], data);
		}
	}
}
/**
* @brief Overloaded function. Sends a json data to all connected players in that instance
* @param instance the instance to send input to
* @param data the json data to send
*/
void UpdateInstance(GameInstance *instance, Json &data)
{
	//update players already in lobby
	for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
	{
		if (instance->players[i])
		{
			SendData(instance->players[i], data);
		}
	}
}
/**
* @brief Overloaded function. Sends a client's input to all connected players in that instance
* @param instance the instance to send input to
* @param data the user input to send
*/
void UpdateInstance(GameInstance *instance, RoVInput &data)
{
	//update players already in lobby
	for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
	{
		if (instance->players[i])
		{
			SendData(instance->players[i], data);
		}
	}
}
/**
* @brief Handles client login. Sends error to client if room is full or that instance already started the game
* @param event stores the client and packet data
* @param data the json data with client data - username / instance number
*/
void onClientLogin(ENetEvent &event, Json &data)
{
	enet_uint32		port = (((int)data["port"]) % 100) + 1; //ports are 1 too 100. Whatever number ppl put will just use the first 3 digits
	GameInstance	*instance = &g_instances[port];
	ENetPeer		*client = event.peer;
	char			 client_ip[32];

	enet_address_get_host_ip(&client->address, client_ip, 32);

	if (instance->is_running)
	{
		Json response;
		ENetPacket *packet;

		SendMessage(client, "Game Already Started", Error);
		return;
	}
	else
	{
		if (!instance->players)
		{
			//allocate memory to track players in instance
			instance->players = (ENetPeer **)malloc(sizeof(ENetPeer*) * MAX_PLAYERS_PER_INSTANCE);
			memset(instance->players, 0, sizeof(ENetPeer*) * MAX_PLAYERS_PER_INSTANCE);
		}
		int i;
		for (i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
		{
			if (!instance->players[i])
			{
				instance->players[i] = client;
				instance->data[i]["username"] = data["username"];
				client->data = (void*)port;
				break;
			}
		}
		if (i == MAX_PLAYERS_PER_INSTANCE)
		{
			printf("Room full\n");
			SendMessage(client, "Room FULL", Error);
		}
		else
		{
			Json instance_info;
			instance_info["event"] = JoinLobby;

			for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
			{
				if (instance->players[i])
				{
					std::string id = "p" + std::to_string(i + 1);
					std::cout << "id is " << id << std::endl;
					instance_info[id] = instance->data[i]["username"];
				}
			}
			//change event to a player joined
			SendData(client, instance_info);

			instance_info["event"] = JoinPlayer;
			//add player to game instance map 
			g_player_instance_map[client] = instance;
			UpdateOthers(instance, client, instance_info);			
		}
	}
}
/**
* @brief Process json data. Used mainly to handle login and lobby menu input
*/
void onReceiveJson(ENetEvent &event)
{
	Json data = Json::parse(&event.packet->data[1]);
	Event game_event = data["event"];
	switch (game_event)
	{
	case Login:
		onClientLogin(event, data);
		break;
	case Chat:
		break;
	case Start:
	{
		Json new_event;
		new_event["event"] = InitGame;
		UpdateInstance(g_player_instance_map[event.peer], new_event);
		break;
	}
	}
}
/**
* @brief Takes a sf::event and sends it to all players in a game instance
* Created for testing. Will be removed later.
* @param event the sf::event user input from a client
*/
void onReceiveSfEvent(ENetEvent &event)
{
	RoVInput input = *(RoVInput*)event.packet->data;

	GameInstance * instance;
	instance = g_player_instance_map[event.peer];

	UpdateInstance(instance, input);
}

/**
* @brief Callback for when packet is received. 
* Checks the first byte for packet type then calls method to process data
*/
void onReceiveData(ENetEvent &event)
{
	//first byte of info is the type
	enet_uint8 type = event.packet->data[0];
	switch ((PacketEnum)type)
	{
	case PacketJson:
	{
		onReceiveJson(event);
		break;
	}
	case PacketSfEvent:
		onReceiveSfEvent(event);
		break;
	default:
		break;
	}
}
/**
* @brief initializses enet and allocates memory for game instances. Sets up enet server host.
*/
void Init()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		exit(1);
	}
	atexit(enet_deinitialize);

	//clear game instances 
	g_instances = (GameInstance *)malloc(sizeof(GameInstance) * MAX_INSTANCES);
	memset(g_instances, 0, sizeof(GameInstance) * MAX_INSTANCES);

	g_address.host = ENET_HOST_ANY;//localhost for testing
	g_address.port = LOGIN_PORT;

	g_server = enet_host_create(&g_address /* the address to bind the server host to */,
		MAX_CLIENTS      /* allow up to MAX clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (g_server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
}

/**
* @brief closes enet
*/
void CloseServer()
{
	enet_host_destroy(g_server);
}

/**
* @brief infinite while loop listening for packets to process. Handles connection, receiving packets, and client disconnections
*/
void ServerLoop()
{
	ENetEvent event;
	int pack_per_sec = 30;
	int time_interval = 1000 / pack_per_sec;
	/* Wait up to 1000 milliseconds for an event. */
	bool done = false;

	while (!done)
	{
		while (enet_host_service(g_server, &event, time_interval) > 0)
		{
			printf("received event\n");
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("Connection \n");

				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				char host_ip[64];
				enet_address_get_host_ip(&event.peer->address, host_ip, 64);
				printf("host ip is %s\n, port is %u", host_ip, event.peer->address.port);

				onReceiveData(event);
				//destroy packet after use
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("Client disconnected\n");
				/* Reset the peer's client information. */
				GameInstance * instance = g_player_instance_map[event.peer];
				if (instance)
				{
					Json update_players;
					update_players["event"] = LeavePlayer;
					for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
					{
						if (instance->players[i])
						{
							if (instance->players[i]->connectID == event.peer->connectID)
							{
								instance->players[i] = NULL;
								instance->data[i].clear();
							}
							else
							{
								std::string id = "p" + std::to_string(i + 1);
								update_players[id] = instance->data[i]["username"];
							}							
						}
					}
					std::cout << "update player \n" << update_players.dump() << std::endl;
					UpdateOthers(instance, event.peer, update_players);
				}
				event.peer->data = NULL;
			}
		}
	}
}

/**
* @brief main entry point. Initializes server, runs loop, then closes server
* @return 0 after closing
*/

int main()
{
	Init();
	ServerLoop();
	CloseServer();
	system("pause");
}