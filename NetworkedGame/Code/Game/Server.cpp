#include "Game/Server.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Game/MultiplayerGame.hpp"

Server::Server(  )
{

}

void Server::AddPlayerClientToServer( Client* client )
{
	m_clients.push_back( client );
}

bool Server::RemoveClient( Client* client )
{
	for ( int i = 0; i < m_clients.size(); i++ )
	{
		if ( m_clients[ i ] == nullptr )
		{
			continue;
		}

		if ( m_clients[ i ] == client )
		{
			delete m_clients[ i ];
			m_clients[ i ] = nullptr;
			return true;
		}
	}

	return false;
}

void Server::CreateSinglePlayerGame()
{
	m_singlePlayerGame = new SinglePlayerGame();
	m_gameState = SINGLEPLAYER_GAME;
}

void Server::CreateMultiPlayerGame()
{
	//Uncomment when added multiplayer game
	//m_singlePlayerGame = new MultiplayerGame();
	m_gameState = MULTIPLAYER_GAME;
}
