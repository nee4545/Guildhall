#include "Engine/Network/TCPClient.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <array>

TCPClient::TCPClient()
{

}

SOCKET TCPClient::Connect( std::string host , int port )
{
	struct addrinfo  addrHintsIn;
	struct addrinfo* pAddrOut = NULL;

	ZeroMemory( &addrHintsIn , sizeof( addrHintsIn ) );
	addrHintsIn.ai_family = AF_INET;
	addrHintsIn.ai_socktype = SOCK_STREAM;
	addrHintsIn.ai_protocol = IPPROTO_TCP;
	

	std::string portNum = std::to_string( port );
	
	int iResult = ::getaddrinfo( host.c_str() , portNum.c_str() , &addrHintsIn , &pAddrOut );

	if ( iResult != 0 )
	{
		return INVALID_SOCKET;
	}

	SOCKET socket = ::socket( pAddrOut->ai_family , pAddrOut->ai_socktype , pAddrOut->ai_protocol );
	if ( socket == INVALID_SOCKET )
	{
		closesocket( socket );
		return INVALID_SOCKET;
		//error
	}

	iResult = ::connect( socket , pAddrOut->ai_addr , ( int ) pAddrOut->ai_addrlen );
	if ( iResult == SOCKET_ERROR )
	{
		closesocket( socket );
		socket = INVALID_SOCKET;
		return INVALID_SOCKET;
	}
	freeaddrinfo( pAddrOut );

	if ( socket == INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}

	u_long winsockmode = 1;
	iResult = ioctlsocket( socket , FIONBIO , &winsockmode );

	if ( iResult == SOCKET_ERROR )
	{
		return INVALID_SOCKET;
	}

	g_theConsole.PrintString(Rgba8(), "Client connected" );

	return socket;
}

void TCPClient::Receive( SOCKET s )
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

void TCPClient::Send( std::string message , SOCKET s )
{
	int iResult = send( s , message.c_str() , (int)message.length() , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		//error
		g_theConsole.PrintString( Rgba8() , "Send Error" );
	}
}

void TCPClient::Send( const char* dataPtr , int length , SOCKET s )
{
	int iResult = ::send( s , dataPtr , static_cast< int > (length) , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		g_theConsole.PrintError("Send Error");
	}
}

bool TCPClient::IsValid()
{
	if ( m_socket != INVALID_SOCKET )
	{
		return true;
	}

	return false;
}
