#include <enet/enet.h>
#include <map>
#include "json.hpp"

//set up list of used ports for game instances
//set up map of client id and client ip addr
ENetAddress g_address;
ENetHost * g_server;

void Init()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
	}
	atexit(enet_deinitialize);

	g_server = enet_host_create(&g_address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (g_server == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("success\n");

}

void CloseServer()
{
	enet_host_destroy(g_server);
}

void runUdpServer(unsigned short port)
{
	ENetEvent event;
	int pack_per_sec = 30;
	int time_interval = 1000 / pack_per_sec;  
	/* Wait up to 1000 milliseconds for an event. */
	char data[] = "Client info";
	while(true)
	{
		while (enet_host_service(g_server, &event, time_interval) > 0)
		{
			switch (event.type)
			{
		case ENET_EVENT_TYPE_CONNECT:
		{
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
			event.peer->data = data;
			break;
		}
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->data,
				event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			break;
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
	runUdpServer(5000);
	CloseServer();
	system("pause");
}
