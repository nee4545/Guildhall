#include "Engine/Network/TCPServer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <array>

TCPServer::TCPServer( int portNum )
{
	m_listenPort = portNum;
	FD_ZERO( &m_listenSet );
	m_listenSocket = INVALID_SOCKET;
	m_timeval.tv_sec = 0l;
	m_timeval.tv_usec = 0l;

}

TCPServer::~TCPServer()
{

}

bool TCPServer::StartServer()
{
	if ( m_listenSocket == INVALID_SOCKET )
	{
		
		ZeroMemory( &addHintsIn , sizeof( addHintsIn ) );
		addHintsIn.ai_family = AF_INET;
		addHintsIn.ai_socktype = SOCK_STREAM;
		addHintsIn.ai_protocol = IPPROTO_TCP;
		addHintsIn.ai_flags = AI_PASSIVE;

		std::string serverPort;
		serverPort = std::to_string( m_listenPort );
		int iResult = getaddrinfo( NULL , serverPort.c_str() , &addHintsIn , &pAddrOut );

		if ( iResult != 0 )
		{
			//error
			return false;
		}

		//SOCKET listenSocket
		m_listenSocket = socket( pAddrOut->ai_family , pAddrOut->ai_socktype , pAddrOut->ai_protocol );

		if ( m_listenSocket == INVALID_SOCKET )
		{
			//error 
			return false;
		}

		unsigned long blockingMode = 1;
		iResult = ioctlsocket( m_listenSocket , FIONBIO , &blockingMode );

		if ( iResult == SOCKET_ERROR )
		{
			//error
			return false;
		}

		
		return true;
	}

	return false;
}

void TCPServer::Listen()
{
	int iResult = ::listen( m_listenSocket , SOMAXCONN );
	if ( iResult == SOCKET_ERROR )
	{
		g_theConsole.PrintString( Rgba8() , "Listen Failed " );
	}

	g_theConsole.PrintString( Rgba8() , "Server Started" );
}

void TCPServer::Bind()
{
	int iResult = ::bind( m_listenSocket , pAddrOut->ai_addr , ( int ) pAddrOut->ai_addrlen );

	if ( iResult == SOCKET_ERROR )
	{
		g_theConsole.PrintString( Rgba8() , "Bind Failed" );
	}

}

SOCKET TCPServer::Accept()
{
	SOCKET socket = INVALID_SOCKET;
	
	FD_ZERO( &m_listenSet );
	FD_SET( m_listenSocket , &m_listenSet );
	int iResult = select( 0 , &m_listenSet , NULL , NULL , &m_timeval );
	if ( iResult == SOCKET_ERROR )
	{
		g_theConsole.PrintString( Rgba8() , "Select Error" );
	}

	if ( FD_ISSET( m_listenSocket , &m_listenSet ) )
	{
		socket = ::accept( m_listenSocket , NULL , NULL );
	}

	if ( socket == INVALID_SOCKET )
	{
		//error
	}

	return socket;
}

void TCPServer::Send( std::string message , SOCKET s )
{
	int iResult = send( s , message.c_str() , (int)message.length() , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		//error
		g_theConsole.PrintString( Rgba8() , "Send Error" );
	}
}

void TCPServer::Send( const char* dataPtr , int length , SOCKET s )
{
	int iResult = ::send( s , dataPtr , static_cast< int > ( length ) , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		g_theConsole.PrintError( "Send Error" );
	}
}

void TCPServer::Receive(SOCKET s)
{
	std::array<char , 256> buffer;
	int iResult = ::recv( s , &buffer[ 0 ] , (int)buffer.size() , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		//closesocket( s );
		//error
		//g_theConsole.PrintError( Stringf( "Error = %d" , WSAGetLastError() ) );
	}
	else if ( iResult == 0 )
	{
		//closesocket( s );
		//error
		//g_theConsole.PrintError( Stringf( "Error = %d" , WSAGetLastError() ) );
	}
	else
	{
		g_theConsole.PrintString( Rgba8( 0 , 0 , 255 , 255 ) , &buffer[ 4 ] );
	}
}

bool TCPServer::IsValid()
{
	if ( m_listenSocket != INVALID_SOCKET )
	{
		return true;
	}

	return false;
}


