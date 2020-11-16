#include "Game/RemoteServer.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/UDPSocket.hpp"
#include "Game/MultiplayerGame.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Timer.hpp"
#include <array>

RemoteServer::RemoteServer()
{
	m_timer = new Timer();
	m_timer->SetSeconds( 0.032 );
}

RemoteServer::~RemoteServer()
{
	writerThread->join();
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
	m_multiplayerGame->Update( deltaSeconds );
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

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		std::string msg = "W|Pressed";
        
		if ( m_timer->HasElapsed() )
		{
			if ( m_UDPSocket != nullptr )
			{
				memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
				m_UDPSocket->Send( strlen( msg.c_str()) );
			}
			m_timer->Reset();
		}
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		std::string msg = "S|Pressed";

		if ( m_timer->HasElapsed() )
		{
			if ( m_UDPSocket != nullptr )
			{
				memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
				m_UDPSocket->Send( strlen( msg.c_str() ) );
			}
			m_timer->Reset();
		}
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		std::string msg = "A|Pressed";

		if ( m_timer->HasElapsed() )
		{
			if ( m_UDPSocket != nullptr )
			{
				memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
				m_UDPSocket->Send( strlen( msg.c_str() ) );
			}
			m_timer->Reset();
		}
	}

	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		std::string msg = "D|Pressed";

		if ( m_timer->HasElapsed() )
		{
			if ( m_UDPSocket != nullptr )
			{
				memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
				m_UDPSocket->Send( strlen( msg.c_str() ) );
			}
			m_timer->Reset();
		}
	}

	if ( g_theInput->WasLeftMouseButtonJustPressed() )
	{
		Vec2 position = m_multiplayerGame->m_mousePosition;
		std::string msg = "Shoot|";
		msg += "PositionX=" + std::to_string( position.x )+"|";
		msg += "PositionY=" + std::to_string( position.y ) + "|";

		if ( m_timer->HasElapsed() )
		{
			if ( m_UDPSocket != nullptr )
			{
				memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , msg.c_str() , strlen( msg.c_str() ) );
				m_UDPSocket->Send( strlen( msg.c_str() ) );
			}
			m_timer->Reset();
		}
	}

	
}

void RemoteServer::EndFrame()
{
	//if ( m_UDPSocket != nullptr )
	//{
	//	if ( m_UDPSocket->Receive() > 0 )
	//	{
	//		m_multiplayerGame->CreateOrUpdateEntitiesFromStr( &m_UDPSocket->ReceiveBuffer()[0] );
	//	}
	//	//std::string msg = m_UDPSocket->ReceiveBuffer().data();
	//	//g_theConsole.PrintString( Rgba8() , m_UDPSocket->ReceiveBuffer().data() );
	//}


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

		writerThread = new std::thread( &RemoteServer::Writer , this , std::ref( *m_UDPSocket ) , std::ref( m_wirteArray ) );
		connectionEstablished = true;
	}

}

Game* RemoteServer::GetGame()
{
	return (Game*)m_multiplayerGame;
}

void RemoteServer::Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writearray )
{
	while ( socket.IsValid() )
	{
		if ( socket.Receive() > 0 )
		{
			auto& buffer = socket.ReceiveBuffer();
			std::string receivedMessage = buffer.data();

			m_multiplayerGame->CreateOrUpdateEntitiesFromStr( receivedMessage );
		}
	}
}
