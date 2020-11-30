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
	m_timer->SetSeconds( 0.032 );
}

AuthoritativeServer::~AuthoritativeServer()
{
	if ( m_singlePlayerGame != nullptr )
	{
		delete m_singlePlayerGame;
		m_singlePlayerGame = nullptr;
	}

	writerThread->join();
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
				msg1 += "PositionY =" + std::to_string( m_multiplayerGame->m_player1->m_position.y )+ "|";
				if ( m_multiplayerGame->m_player1->m_isGarbage )
				{
					msg1 += "IsGarbage =true";
					msg1 += "|";
				}
				else
				{
					msg1 += "IsGarbage =false";
					msg1 += "|";
				}

				msg1 += "Health =" + std::to_string( m_multiplayerGame->m_player1->m_health ) + "|";

				if ( m_multiplayerGame->m_player2 != nullptr )
				{
					std::string msg2 = "ID =";
					msg2 += std::to_string( m_multiplayerGame->m_player2->m_ID ) + "|";
					msg2 += "EntityType = Player|";
					msg2 += "Faction = Bad|";
					msg2 += "PositionX =" + std::to_string( m_multiplayerGame->m_player2->m_position.x ) + "|";
					msg2 += "PositionY =" + std::to_string( m_multiplayerGame->m_player2->m_position.y ) + "|";
					if ( m_multiplayerGame->m_player2->m_isGarbage )
					{
						msg2 += "IsGarbage =true";
						msg2 += "|";
					}
					else
					{
						msg2 += "IsGarbage =false";
						msg2 += "|";
					}
					msg2 += "Health =" + std::to_string( m_multiplayerGame->m_player2->m_health ) + "|";
					messages.push_back( msg2 );
				}

				messages.push_back( msg1 );

				for ( int i = 0; i < m_multiplayerGame->m_bullets.size(); i++ )
				{
					if ( m_multiplayerGame->m_bullets[ i ] != nullptr )
					{
						std::string msg = "ID =";
						msg += std::to_string( m_multiplayerGame->m_bullets[ i ]->m_ID ) + "|";
						msg += "EntityType=Bullet|";
						if ( m_multiplayerGame->m_bullets[ i ]->m_faction == FACTION_GOOD )
						{
							msg += "Faction=Good|";
						}
						else
						{
							msg += "Faction=Bad|";
						}
						msg += "PositionX =" + std::to_string( m_multiplayerGame->m_bullets[ i ]->m_position.x ) + "|" ;
						msg += "PositionY =" + std::to_string( m_multiplayerGame->m_bullets[ i ]->m_position.y ) + "|";
						
						if ( m_multiplayerGame->m_bullets[ i ]->m_isGarbage )
						{
							msg += "IsGarbage =true";
							msg += "|";
						}
						else
						{
							msg += "IsGarbage =false";
							msg += "|";
						}
						msg += "Health =" + std::to_string( m_multiplayerGame->m_player2->m_health ) + "|";

						messages.push_back( msg );
					}
					
				}

				for ( int i = 0; i < m_multiplayerGame->m_ais.size(); i++ )
				{
					if ( m_multiplayerGame->m_ais[ i ] != nullptr )
					{
						std::string msg = "ID =";
						msg += std::to_string( m_multiplayerGame->m_ais[ i ]->m_ID ) + "|";
						msg += "EntityType=AI|";
						if ( m_multiplayerGame->m_ais[ i ]->m_faction == FACTION_GOOD )
						{
							msg += "Faction=Good|";
						}
						else
						{
							msg += "Faction=Bad|";
						}
						msg += "PositionX =" + std::to_string( m_multiplayerGame->m_ais[ i ]->m_position.x ) + "|";
						msg += "PositionY =" + std::to_string( m_multiplayerGame->m_ais[ i ]->m_position.y ) + "|";
						if ( m_multiplayerGame->m_ais[ i ]->m_isGarbage )
						{
							msg += "IsGarbage =true";
							msg += "|";
						}
						else
						{
							msg += "IsGarbage =false";
							msg += "|";
						}
						msg += "Health =" + std::to_string( m_multiplayerGame->m_player2->m_health ) + "|";
						messages.push_back( msg );
					}
				}

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
	//m_multiplayerGame->CreateSecondPlayer();
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
	std::string address = GetAddress();
	Strings s = SplitStringOnDelimiter( address , ':' );
	if ( s[ 0 ] == "127.0.0.1" )
	{
		address = "127.1.1.1";
	}
	else
	{
		address = s[ 0 ];
	}

	m_UDPSocket = new UDPSocket( address , clienSendPort);
	m_UDPSocket->Bind( m_listenPort );
	m_multiplayerGame->CreateSecondPlayer();

	writerThread = new std::thread( &AuthoritativeServer::Writer , this , std::ref( *m_UDPSocket ) , std::ref( m_wirteArray ) );
	return true;
}

void AuthoritativeServer::SendReliableUDPMessages()
{
	RealiableSendUDPMessage UDPMessage;
	UDPMessage.frameNumber = m_frameNumber;

	std::string msg1 = "ID =";
	msg1 += std::to_string( m_multiplayerGame->m_player1->m_ID ) + "|";
	msg1 += "EntityType=Player|";
	msg1 += "Faction=Good|";
	msg1 += "PositionX=" + std::to_string( m_multiplayerGame->m_player1->m_position.x ) + "|";
	msg1 += "PositionY =" + std::to_string( m_multiplayerGame->m_player1->m_position.y );

	UDPMessage.message.push_back( msg1 );

	if ( m_multiplayerGame->m_player2 != nullptr )
	{
		std::string msg2 = "ID =";
		msg2 += std::to_string( m_multiplayerGame->m_player2->m_ID ) + "|";
		msg2 += "EntityType = Player|";
		msg2 += "Faction = Bad|";
		msg2 += "PositionX =" + std::to_string( m_multiplayerGame->m_player2->m_position.x ) + "|";
		msg2 += "PositionY =" + std::to_string( m_multiplayerGame->m_player2->m_position.y );
		UDPMessage.message.push_back( msg2 );
	}
	
}

void AuthoritativeServer::Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writearray )
{
	while ( socket.IsValid() )
	{
		if ( socket.Receive() > 0 )
		{
			auto& buffer = socket.ReceiveBuffer();
			std::string receivedMessage = buffer.data();

			Strings s = SplitStringOnDelimiter( receivedMessage , '|' );
			if ( s[0] == "W" )
			{
				m_multiplayerGame->m_player2->MoveUp();
				//writearray.push( receivedMessage );
			}

			if ( s[ 0 ] == "S" )
			{
				m_multiplayerGame->m_player2->MoveDown();
				//writearray.push( receivedMessage );
			}

			if ( s[ 0 ] == "A" )
			{
				m_multiplayerGame->m_player2->MoveLeft();
				//writearray.push( receivedMessage );
			}

			if ( s[ 0 ] == "D" )
			{
				m_multiplayerGame->m_player2->MoveRight();
				//writearray.push( receivedMessage );
			}

			if ( s[ 0 ] == "Shoot" )
			{
				Vec2 position;
				Strings s1 = SplitStringOnDelimiter( s[ 1 ] , '=' );
				Strings s2 = SplitStringOnDelimiter( s[ 2 ] , '=' );
				position.x = atof( s1[ 1 ].c_str() );
				position.y = atof( s2[ 1 ].c_str() );

				Vec2 forwardVec = ( position - m_multiplayerGame->m_player2->m_position ).GetNormalized();
				m_multiplayerGame->SpawnBullet( m_multiplayerGame->m_player2->m_position , forwardVec , FACTION_BAD );
			}

			//g_theConsole.PrintString( Rgba8() , receivedMessage );
		}
	}
}

void AuthoritativeServer::AddRealiableUDPMessagesToArray()
{
	
}

std::string AuthoritativeServer::GetAddress()
{
	std::array<char , 128> addressStr;

	sockaddr clientAddr;
	int addrSize = sizeof( clientAddr );

	int iResult = getpeername( m_clientSocket , &clientAddr , &addrSize );

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}

	DWORD outlen = ( DWORD ) addressStr.size();
	iResult = WSAAddressToStringA( &clientAddr , addrSize , NULL , &addressStr[ 0 ] , &outlen );

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}

	return &addressStr[ 0 ];
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
