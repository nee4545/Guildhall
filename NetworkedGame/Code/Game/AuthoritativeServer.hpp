#pragma once
#include "Game/Server.hpp"
#include "Engine/Core/SynchronizedLockFreeQueue.hpp"
#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif //  _WINSOCK_DEPRECATED_NOWARNINGS

#include <winsock2.h>
#include "ws2tcpip.h"
#include <thread>

class RandomNumberGenerator;
class MultiplayerGame;
class SinglePlayerGame;
class InputSystem;
class TCPServer;
class TCPClient;
class UDPSocket;
class Timer;

struct TCPMessage
{
	int uniqueId =0;
	int sendPortNumber =0;
};

struct ConnectedClients
{
	int clientId;
	int clientPort;
};

class AuthoritativeServer :public Server
{
public:


	AuthoritativeServer( );
	~AuthoritativeServer();

public:
	void StartUp();
	void ShutDown();
	void Update( float deltaSeconds );
	void BeginFrame();
	void EndFrame();
	void StartMultiplayerGame(int portNum);
	bool EstablishRemoteConnection();

	void Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writearray );

	Game* GetGame();
	void AddPlayer();
	void AssignInputSystem(InputSystem* inputSystem);
	bool m_currentlySinglePlayer = true;
	
	TCPServer* m_server = nullptr;
	SOCKET m_clientSocket = INVALID_SOCKET;

	UDPSocket* m_UDPSocket = nullptr;

	std::vector<ConnectedClients> m_connectedClients;
	RandomNumberGenerator* rng = nullptr;

	int m_listenPort = 48000;
	SynchronizedLockFreeQueue < std::string > m_wirteArray;

	Timer* m_timer = nullptr;
	std::thread* writerThread = nullptr;
};


