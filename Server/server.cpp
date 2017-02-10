#include <enet/enet.h>
#include <map>
#include <iostream>
#include "json.hpp"

//set up list of used ports for game instances
//set up map of client id and client ip addr
ENetAddress g_address;
ENetHost * g_server;

#define SERVER_IP "rov.garryguan.com"
#define LOGIN_PORT 5000

#define MAX_CLIENTS 400
#define MAX_INSTANCES 100
#define MAX_PLAYERS_PER_INSTANCE 4

using Json = nlohmann::json;
//Struct containg all the players in a game instance
typedef struct
{
	bool is_running; // false =  lobby, true =  game instance
	ENetPeer		**players;
	Json			data[MAX_PLAYERS_PER_INSTANCE];
}GameInstance;

enum Event
{
	Error,
	Login,
	JoinLobby,
	Input,
	Chat,
	Start
};

GameInstance *g_instances; // indexed by port #

void SendData(ENetPeer *player, Json &data)
{
	ENetPacket *packet = enet_packet_create(data.dump().c_str(), data.dump().length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(player, 1, packet);
}

void SendMessage(ENetPeer *player, std::string message, Event event)
{
	Json response;
	response["event"] = event;
	response["msg"] = message;


	ENetPacket *packet = enet_packet_create(response.dump().c_str(), response.dump().length() + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(player, 1, packet);
}

void onClientLogin(ENetEvent event, Json &data)
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

			instance_info["event"] = Login;
			for (int i = 0; i < MAX_PLAYERS_PER_INSTANCE; i++)
			{
				ENetPeer *player = instance->players[i];
				if (player)
				{
					instance_info["players"].push_back(instance->data[i]["username"]);
				}
			}
			SendData(client, instance_info);
		}
	}
}

void onClientReceiveData(ENetEvent event)
{
	ENetPeer * player = event.peer;
	ENetPacket *packet = event.packet;
	Json data = Json::parse(packet->data);

	Event game_event = data["type"];

	switch (game_event)
	{
	case Login:
		onClientLogin(event, data);
		break;
	}


}

//set server ip and config
void InitServer()
{
	g_address.host = ENET_HOST_ANY;//localhost for testing
	g_address.port = LOGIN_PORT;

	g_server = enet_host_create(&g_address /* the address to bind the server host to */,
		MAX_CLIENTS      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (g_server == NULL)
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
}

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

	InitServer();
}

void CloseServer()
{
	enet_host_destroy(g_server);
}

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
				printf("host ip is %s\n", host_ip);

				onClientReceiveData(event);
				//destroy packet after use
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", event.peer->data);
				/* Reset the peer's client information. */
				event.peer->data = NULL;
			}
		}
	}
}

int main()
{
	Init();
	ServerLoop();
	CloseServer();
	system("pause");
}