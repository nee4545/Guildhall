#include "Game/AuthoritativeServer.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Game/Player.hpp"
#include "Game/AI.hpp"
#include "Game/Bullet.hpp"
#include "Game/MultiplayerGame.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/UDPSocket.hpp"
#include "Engine/Core/Timer.hpp"

AuthoritativeServer::AuthoritativeServer(  )
{
	rng = new RandomNumberGenerator();
	m_timer = new Timer();
	m_timer->SetSeconds( 0.05 );
}

AuthoritativeServer::~AuthoritativeServer()
{
	if ( m_singlePlayerGame != nullptr )
	{
		delete m_singlePlayerGame;
		m_singlePlayerGame = nullptr;
	}

	
}

void AuthoritativeServer::StartUp()
{
	
}

void AuthoritativeServer::ShutDown()
{

}

void AuthoritativeServer::Update( float deltaSeconds )
{
	if ( m_gameState == SINGLEPLAYER_GAME )
	{
		m_singlePlayerGame->Update( deltaSeconds );
		
	}

	if ( m_gameState == MULTIPLAYER_GAME )
	{
		m_multiplayerGame->Update( deltaSeconds );
	}
}

void AuthoritativeServer::BeginFrame()
{
	if ( m_server != nullptr )
	{
		if ( m_server->IsValid() )
		{
			if ( m_clientSocket == INVALID_SOCKET )
			{
				m_clientSocket = m_server->Accept();

				if ( m_clientSocket != INVALID_SOCKET )
				{
					g_theConsole.PrintString( Rgba8() , "Client Requested to join" );
					EstablishRemoteConnection();

				}
			}
		}
	}

	if ( m_UDPSocket != nullptr )
	{
		if ( m_multiplayerGame != nullptr )
		{
			if ( m_timer->HasElapsed() )
			{
			std::vector<std::string> messages;

				std::string msg1 = "ID =";
				msg1 += std::to_string( m_multiplayerGame->m_player1->m_ID ) + "|";
				msg1 += "EntityType=Player|";
				msg1 += "Faction=Good|";
				msg1 += "PositionX=" + std::to_string( m_multiplayerGame->m_player1->m_position.x ) + "|";
				msg1 += "PositionY =" + std::to_string( m_multiplayerGame->m_player1->m_position.y );

				std::string msg2 = "ID =";
				msg2 += std::to_string( m_multiplayerGame->m_player2->m_ID ) + "|";
				msg2 += "EntityType = Player|";
				msg2 += "Faction = Bad|";
				msg2 += "PositionX =" + std::to_string( m_multiplayerGame->m_player2->m_position.x ) + "|";
				msg2 += "PositionY =" + std::to_string( m_multiplayerGame->m_player2->m_position.y );

				messages.push_back( msg1 );
				messages.push_back( msg2 );

				for ( int i = 0; i < messages.size(); i++ )
				{
					m_wirteArray.push( messages[ i ] );

					memcpy( &( m_UDPSocket->SendBuffer()[ 0/*sizeof( messages[ i ] ) ]*/ ] ) , messages[ i ].c_str() , strlen( messages[ i ].c_str() ) );
					if ( m_UDPSocket != nullptr )
					{
						m_UDPSocket->Send( strlen( messages[ i ].c_str() ) );
					}
				}

				m_timer->Reset();
			}

			
		}
	}
}

void AuthoritativeServer::EndFrame()
{

}

void AuthoritativeServer::StartMultiplayerGame( int portNum )
{
	m_multiplayerGame = new MultiplayerGame();
	m_multiplayerGame->CreatePlayer();
	m_multiplayerGame->CreateSecondPlayer();
	m_multiplayerGame->AssignInputSystemForPlayer1( g_theInput );
	m_gameState = MULTIPLAYER_GAME;
	m_server = new TCPServer(portNum);

	m_server->StartServer();
	m_server->Bind();
	m_server->Listen();
	
}

bool AuthoritativeServer::EstablishRemoteConnection()
{
	int clientID = rng->RollRandomIntInRange( 0 , 100 );
	int clienSendPort = rng->RollRandomIntInRange( 46000 , 50000 );
	std::string message = "ClientID=" + std::to_string( clientID ) + "|" + "ClientBindPort=" + std::to_string( clienSendPort );
	m_server->Send( message , m_clientSocket );
	ConnectedClients c;
	c.clientId = clientID;
	c.clientPort = clienSendPort;
	m_connectedClients.push_back( c );

	m_UDPSocket = new UDPSocket( "127.1.1.1" , clienSendPort);
	m_UDPSocket->Bind( m_listenPort );

	writerThread = new std::thread( &AuthoritativeServer::Writer , this , std::ref( *m_UDPSocket ) , std::ref( m_wirteArray ) );
	return true;
}

void AuthoritativeServer::Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writearray )
{
	while ( socket.IsValid() )
	{
		if ( socket.Receive() > 0 )
		{
			auto& buffer = socket.ReceiveBuffer();
			std::string receivedMessage = buffer.data();

			if ( receivedMessage == "W" )
			{
				m_multiplayerGame->m_player2->MoveUp();
			}

			g_theConsole.PrintString( Rgba8() , receivedMessage );
		}
	}
}

Game* AuthoritativeServer::GetGame()
{
	if ( m_gameState == SINGLEPLAYER_GAME )
	{
		return m_singlePlayerGame;
	}
	else
	{
		return m_multiplayerGame;
	}
}

void AuthoritativeServer::AddPlayer()
{
	if ( m_gameState == SINGLEPLAYER_GAME )
	{
		m_singlePlayerGame->StartUp();
		m_singlePlayerGame->AssignInputSystem( g_theInput );
		m_singlePlayerGame->CreatePlayer();
	}
}

void AuthoritativeServer::AssignInputSystem( InputSystem* inputSystem )
{
	if ( m_gameState == SINGLEPLAYER_GAME )
	{
		m_singlePlayerGame->AssignInputSystem( inputSystem );
	}
}
