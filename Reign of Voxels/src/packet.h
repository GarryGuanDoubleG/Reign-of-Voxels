#pragma once
#include <enet\enet.h>
#include <SFML\Graphics.hpp>
#include "json.hpp"
using Json = nlohmann::json; /**< convenience specified by nlohmann himself */  
//first byte of the packet used for intepretting value
enum PacketEnum
{
	PacketJson = 1, /**<enum Packet contains json data */  
	PacketSfEvent, /**<enum packet contains sf event  */  
};
/**
* This was created to establish multi frustum control between clients for prototyping.
* This will be removed later once command inputs are established
*/
typedef struct RoVInput
{
	enet_uint8 type; /**<single byte to indicate how to interpret packet */  
	sf::Event event; /**<user input*/  
};  