#pragma once

enum Event
{
	Error, /**< Server has encountered an error */  
	Login, /**< Player starts to login */  
	JoinLobby, /**< Login succeeds and joins a lobby */  
	JoinPlayer, /**< Another player joins same lobby  */  
	LeavePlayer, /**< Someone in lobby left */  
	ClientInput, /**< Player input locally */  
	ServerInput,/**<  Player input sent from server*/ 
	BuildButton,
	UnitButton,
	Sound,
	Music,
	Chat, /**<  Player typed something in chat*/  
	Start, /**< Player starts the game*/  
	InitGame, /**< Server response to inform all players to initiate game*/  
	Close /**< Close the game and start cleaning up*/  
};