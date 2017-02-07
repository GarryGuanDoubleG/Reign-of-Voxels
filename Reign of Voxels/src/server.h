#pragma once
#ifndef _SERVER_H
#define _SERVER_H


#define MAX_CLIENTS 100;

class Server
{
private:

public:
	Server();
	~Server();

	void SendData();
	void onReceiveData();
	void ServerLoop();
};
#endif