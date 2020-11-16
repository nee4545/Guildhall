#include "Game/RemoteServer.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/UDPSocket.hpp"
#include "Game/MultiplayerGame.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include <array>

RemoteServer::RemoteServer()
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::StartUp()
{
	m_multiplayerGame = new MultiplayerGame();
	//m_multiplayerGame->CreatePlayer();
	//m_multiplayerGame->CreateSecondPlayer();

}

void RemoteServer::ShutDown()
{

}

void RemoteServer::Update( float deltaSeconds )
{

}

void RemoteServer::BeginFrame()
{
	if ( m_TCPclient != nullptr )
	{
		if(m_TCPclient->IsValid())
		{
			//m_TCPclient->Receive( m_TCPclient->m_socket );
			
			ReceiveTCPMessageFromServer();
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'W' ) )
	{
		std::string msg = "W";

		if ( m_UDPSocket != nullptr )
		{
			memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
			m_UDPSocket->Send( strlen( msg.c_str() ) );
		}
	}

	
}

void RemoteServer::EndFrame()
{
	if ( m_UDPSocket != nullptr )
	{
		if ( m_UDPSocket->Receive() > 0 )
		{
			m_multiplayerGame->CreateOrUpdateEntitiesFromStr( &m_UDPSocket->ReceiveBuffer()[0] );
		}
		//std::string msg = m_UDPSocket->ReceiveBuffer().data();
		//g_theConsole.PrintString( Rgba8() , m_UDPSocket->ReceiveBuffer().data() );
	}

	if ( g_theInput->WasKeyJustPressed( 'W' ) )
	{
		std::string msg = "Wadasd";
		
		if ( m_UDPSocket != nullptr )
		{
			memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
			m_UDPSocket->Send( strlen(msg.c_str()) );
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'S' ) )
	{
		std::string msg = "S";

		if ( m_UDPSocket != nullptr )
		{
			memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
			m_UDPSocket->Send( 1 );
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'A' ) )
	{
		std::string msg = "A";

		if ( m_UDPSocket != nullptr )
		{
			memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
			m_UDPSocket->Send( 1 );
		}
	}

	if ( g_theInput->WasKeyJustPressed( 'D' ) )
	{
		std::string msg = "D";

		if ( m_UDPSocket != nullptr )
		{
			memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
			m_UDPSocket->Send( 1 );
		}
	}
}

void RemoteServer::StartMultiplayerGame( std::string address, int portNum )
{
	m_TCPclient = new TCPClient( );
	m_TCPclient->m_socket=m_TCPclient->Connect( address , portNum );
	//m_TCPclient->Receive( m_TCPclient->m_socket );
	ReceiveTCPMessageFromServer();

}

bool RemoteServer::EstablishRemoteConnection()
{
	return false;
}

void RemoteServer::ReceiveTCPMessageFromServer()
{
	std::array<char , 256> buffer;

	int iResult = ::recv( m_TCPclient->m_socket , &buffer[ 0 ] , ( int ) buffer.size() , 0 );

	if ( iResult == SOCKET_ERROR )
	{
		//g_theConsole.PrintError( "Client Socket Send Error" );
	}
	else
	{
		g_theConsole.PrintString( Rgba8() , &buffer[ 0 ]);
		Strings s = SplitStringOnDelimiter( &buffer[ 0 ] , '|' );

		Strings s1 = SplitStringOnDelimiter( s[ 0 ] , '=' );
		m_Id = atoi( s1[ 1 ].c_str() );
		
		Strings s2 = SplitStringOnDelimiter( s[ 1 ] , '=' );
		m_serverSendPort = atoi( s2[ 1 ].c_str() );

		//g_theConsole.PrintString( Rgba8() , std::to_string( m_Id ) );
		//g_theConsole.PrintString( Rgba8() , std::to_string( m_serverSendPort ) );

		m_UDPSocket = new UDPSocket( "127.1.1.1" , 48000 );
		m_UDPSocket->Bind( m_serverSendPort );

		connectionEstablished = true;
	}

}

Game* RemoteServer::GetGame()
{
	return (Game*)m_multiplayerGame;
}
