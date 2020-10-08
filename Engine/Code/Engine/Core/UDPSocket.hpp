#pragma once
#ifndef  _WINSOCK_DEPRECATED_NO_WARNINGS
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include <winsock2.h>
#include "ws2tcpip.h"
#include <array>
#include <string>

class UDPSocket
{
public:
	static const int BufferSize = 512;
	using Buffer = std::array<char , BufferSize>;

	UDPSocket( const std::string& host , int port );
	~UDPSocket();

	void Bind( int port );
	int Send( int length );
	int Receive();
	void Close();

	Buffer& SendBuffer();
	Buffer& ReceiveBuffer();

	bool IsValid();

private:
	Buffer m_sendBuffer;
	Buffer m_receiveBuffer;
	sockaddr_in m_toddress;
	sockaddr_in m_bindAddress;
	SOCKET m_socket = INVALID_SOCKET;
};