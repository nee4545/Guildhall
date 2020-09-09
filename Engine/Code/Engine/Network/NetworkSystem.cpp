#include "Engine/Network/NetworkSystem.hpp"
#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>


NetworkSystem::NetworkSystem()
{

}

NetworkSystem::~NetworkSystem()
{

}

void NetworkSystem::StartUp()
{
	WSADATA wsaData;
	WORD wVersion MAKEWORD( 2 , 2 );
	int iResult = WSAStartup( wVersion , &wsaData );

	if ( iResult != 0 )
	{
		//error
	}

}

void NetworkSystem::ShutDown()
{
	int iResult = WSACleanup();

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}
}

void NetworkSystem::BeginFrame()
{
	if ( m_isListening )
	{
		if ( m_listenSocket == INVALID_SOCKET )
		{
			struct addrinfo addHintsIn;
			struct addrinfo* pAddrOut;

			ZeroMemory( &addHintsIn , sizeof( addHintsIn ) );
			addHintsIn.ai_family = AF_INET;
			addHintsIn.ai_socktype = SOCK_STREAM;
			addHintsIn.ai_protocol = IPPROTO_TCP;
			addHintsIn.ai_flags = AI_PASSIVE;

			std::string serverPort( "48000" );
			int iResult = getaddrinfo( NULL , serverPort.c_str() , &addHintsIn , &pAddrOut );

			if ( iResult != 0 )
			{
				//Error
			}

			//SOCKET listenSocket
			m_listenSocket = socket( pAddrOut->ai_family , pAddrOut->ai_socktype , pAddrOut->ai_protocol );

			if ( m_listenSocket == INVALID_SOCKET )
			{
				//error
			}

			unsigned long blockingMode = 1;
			iResult = ioctlsocket( m_listenSocket , FIONBIO , &blockingMode );

			if ( iResult == SOCKET_ERROR )
			{
				//error
			}

			iResult = bind( m_listenSocket , pAddrOut->ai_addr , ( int ) pAddrOut->ai_addrlen );

			if ( iResult == SOCKET_ERROR )
			{
				//error
			}

			iResult = listen( m_listenSocket , SOMAXCONN );
			if ( iResult == SOCKET_ERROR )
			{
				//error
			}
		}
	}
}
