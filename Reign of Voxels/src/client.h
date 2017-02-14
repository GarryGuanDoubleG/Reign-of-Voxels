#ifndef _CLIENT_H_
#define _CLIENT_H_
#include <enet\enet.h>
#include <SFML\Graphics.hpp>
#include <SFML/System.hpp>
#include "EventSystem.h"
/**
* server static ip address
*/
#define SERVER_IP_ADDR "104.192.170.117"
/**
* server port to send packets to
*/
#define SERVER_PORT 5000
/**
* Manages network connection with server
*/
class Client : public Observer
{
public:
	/**
	* constructor
	* initializes enet and creates enet host and subscribes to global event system
	*/
	Client();
	/**
	* Destructor 
	* unsubscribes from event system and deinitializes enet
	*/
	~Client();
	/**
	* @brief callback function when game event occurs
	* @param event type of event
	* @param obj data associated with event. Can be null
	*/
	virtual void onNotify(Event event, Json &obj);
	/**
	* @brief callback function when user inputs
	* @param event type of event
	* @param input the user input
	*/
	virtual void onNotify(Event event, sf::Event &input);
	/**
	* @brief Connects to server with user and room #
	*/
	void ConnectHost();
	/**
	* @brief sends a json object to the server
	* @param obj the json object to send
	*/
	void SendData(Json &obj);
	/**
	* @brief sends user input to the server
	* @param event the user input to send
	*/
	void SendInput(sf::Event event);
	/**
	* @brief Thread launched when logged in. Runs a loop that checks for server packets
	*/
	void ConnectionEvent();
	/**
	* @brief manages the data send from the server
	* @param event the enet container for packet and packet information
	*/
	void onReceiveData(ENetEvent event);

	std::string		m_username; /**<name player chose on login */  
	sf::Uint32		m_port; /**< room instance player chose */  
private:
	bool			m_game_started; /**< flag for if game started */  
	bool			m_connected; /**< flag for if client is connected to server  */  
	sf::Thread		m_thread; /**< thread to launch that waits for server packets */  
	ENetHost		*m_client; /**< enet client used to manage connection with server*/  
	ENetPeer		*m_server; /**< enet server to connect to */  
};

#endif