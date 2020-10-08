#pragma once

#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif //  _WINSOCK_DEPRECATED_NOWARNINGS

#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>
//class SOCKET;

#include "Engine/Core/EngineCommon.hpp"

class TCPServer;
class TCPClient;
class UDPListner;



class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

	void StartUp();
	void ShutDown();
	void BeginFrame();
	void EndFrame();
	std::string GetAddress();

	void CreateTCPServer(int portNum = 48000);
	void CreateTCPClient(std::string address);
	void SendMessageFromServer( std::string message );
	void SendMessageFromClient( std::string message );

	static bool StartServer( EventArgs& args );
	static bool Connect( EventArgs& args );
	static bool SendServerMessage( EventArgs& args );
	static bool SendClientMessage( EventArgs& args );
	static bool StopServ( EventArgs& args );
	static bool Disconnect( EventArgs& args );
	static bool OpenUDPPort( EventArgs& args );
	static bool SendUDPMessage( EventArgs& args );
	static bool CloseUDPPort( EventArgs& args );


	void StopServer();
	void StopClient();
	void StartUDPListner(int bindPort, int sendPort, std::string host = "127.0.0.1");
	void SendUDPMessage( std::string message );

	int m_listenPort = -1;
	bool m_isListening = true;
	FD_SET m_listenSet;
	struct timeval m_timeval;

	unsigned long long m_clientSocket;

	TCPServer* m_server;
	TCPClient* m_client;

	UDPListner* m_UDPlistner = nullptr;
};