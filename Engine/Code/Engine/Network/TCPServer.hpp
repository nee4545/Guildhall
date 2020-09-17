#pragma once
#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>

class TCPServer
{
public:
	TCPServer( int portNum );
	~TCPServer();

	bool StartServer();
	void Listen();
	void Bind();
	SOCKET Accept();
	void Send( std::string message , SOCKET s );
	void Send( const char* dataPtr , int length , SOCKET s );
	void Receive(SOCKET s);
	bool IsValid();

public:
	int m_listenPort;
	FD_SET m_listenSet;
	SOCKET m_listenSocket;
	timeval m_timeval;

	struct addrinfo addHintsIn;
	struct addrinfo* pAddrOut;
};