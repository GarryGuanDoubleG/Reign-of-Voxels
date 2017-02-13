#pragma once

#include <enet\enet.h>
#include <SFML\Graphics.hpp>
#include "json.hpp"

using Json = nlohmann::json;
//first byte of the packet used for intepretting value
enum PacketEnum
{
	PacketJson = 1,
	PacketSfEvent, //useless to send sf events but will do it just for testing
};

typedef struct RoVInput
{
	enet_uint8 type; // how to decode packet
	sf::Event event;
};  
#pragma pack(push, 1)
typedef struct RoVJson
{
	enet_uint8 type; // how to decode packet
	unsigned int length;
	std::string data;
};
#pragma pack(pop)