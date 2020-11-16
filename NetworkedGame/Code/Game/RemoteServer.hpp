#pragma once
#include "Game/Server.hpp"
#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif //  _WINSOCK_DEPRECATED_NOWARNINGS

#include <winsock2.h>
#include "ws2tcpip.h"
#include <thread>
#include "Engine/Core/SynchronizedLockFreeQueue.hpp"

class RandomNumberGenerator;
class MultiplayerGame;
class SinglePlayerGame;
class InputSystem;
class TCPServer;
class TCPClient;
class UDPSocket;
class Timer;

class RemoteServer :public Server
{
public:
	RemoteServer();
	~RemoteServer();

public:
	void StartUp();
	void ShutDown();
	void Update( float deltaSeconds );
	void BeginFrame();
	void EndFrame();
	void StartMultiplayerGame( std::string address , int portNum );
	bool EstablishRemoteConnection();
	void ReceiveTCPMessageFromServer();
	Game* GetGame();

	void Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writearray );

	TCPClient* m_TCPclient = nullptr;
	UDPSocket* m_UDPSocket = nullptr;

	int m_Id;
	int m_serverSendPort;
	bool connectionEstablished = false;

	Timer* m_timer = nullptr;
	std::thread* writerThread = nullptr;
	SynchronizedLockFreeQueue < std::string > m_wirteArray;
};