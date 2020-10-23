#include "Game/AuthoritativeServer.hpp"
#include "Game/SinglePlayerGame.hpp"
#include "Game/MultiplayerGame.hpp"

AuthoritativeServer::AuthoritativeServer(  )
{
	
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
}

void AuthoritativeServer::BeginFrame()
{

}

void AuthoritativeServer::EndFrame()
{

}

Game* AuthoritativeServer::GetGame()
{
	return m_singlePlayerGame;
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
