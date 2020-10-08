#include "Engine/Core/UDPSocket.hpp"
#include "Engine/Core/EngineCommon.hpp"

UDPSocket::UDPSocket( const std::string& host , int port ):m_socket(INVALID_SOCKET)
{
	m_toddress.sin_family = AF_INET;
	m_toddress.sin_port = htons( ( u_short ) port );
	m_toddress.sin_addr.s_addr = inet_addr( host.c_str() );

	m_socket = ::socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP );
	if ( m_socket == INVALID_SOCKET )
	{
		LOG_ERROR( "Socket Creation Failed" );
	}
	else
	{
		g_theConsole.PrintString( Rgba8() , "UDP Socket Started on " + host + " On port " + std::to_string( port ) );
	}
}

UDPSocket::~UDPSocket()
{
	//if ( m_socket != INVALID_SOCKET )
	//{
	//	int result =closesocket( m_socket );
	//	if ( result == INVALID_SOCKET )
	//	{
	//		//error
	//	}
	//}
	//m_socket = INVALID_SOCKET;
	Close();

}

void UDPSocket::Bind( int port )
{
	sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons( ( u_short ) port );
	bindAddr.sin_addr.s_addr = htonl( INADDR_ANY );

	int result = ::bind( m_socket , ( SOCKADDR* ) &bindAddr , sizeof( bindAddr ) );

	
	if ( result != 0 )
	{
		LOG_ERROR( "Bind Failed" );
	}
	else
	{
		g_theConsole.PrintString( Rgba8() , "Socket binded to: " + std::to_string( port ) );
	}
}

int UDPSocket::Send( int length )
{
	int result = ::sendto( m_socket , &m_sendBuffer[ 0 ] , static_cast< int >( length ) , 0 , reinterpret_cast< SOCKADDR* >( &m_toddress ) , sizeof( m_toddress ) );
	if ( result == SOCKET_ERROR )
	{
		//error
	}
	return result;
}

int UDPSocket::Receive()
{
	sockaddr_in fromAddr;
	int fromLen = sizeof( fromAddr );
	int result = ::recvfrom( m_socket , &m_receiveBuffer[ 0 ] , static_cast< int > ( m_receiveBuffer.size() ) , 0 , reinterpret_cast< SOCKADDR* >( &fromAddr ) , &fromLen );

	if ( result == SOCKET_ERROR || result < 0 )
	{
		//error
	}

	return result;
}

void UDPSocket::Close()
{
	if ( m_socket != INVALID_SOCKET )
	{
		int result = closesocket( m_socket );
		if ( result == INVALID_SOCKET )
		{
			LOG_ERROR( "Socket Close Error" );
		}
		else
		{
			g_theConsole.PrintString( Rgba8() , "Socket Closed" );
		}
	}
	m_socket = INVALID_SOCKET;
}

UDPSocket::Buffer& UDPSocket::SendBuffer()
{
	return m_sendBuffer;
}

UDPSocket::Buffer& UDPSocket::ReceiveBuffer()
{
	return m_receiveBuffer;
}

bool UDPSocket::IsValid()
{
	return m_socket != INVALID_SOCKET;
}
