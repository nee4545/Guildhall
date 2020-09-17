#pragma once
#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>

class TCPClient
{
public:
	TCPClient();
public:
	SOCKET Connect( std::string host , int port );
	void Receive( SOCKET s );
	void Send( std::string message , SOCKET s );
	void Send( const char* dataPtr , int length, SOCKET s );
	SOCKET m_socket = INVALID_SOCKET;
	bool IsValid();
};