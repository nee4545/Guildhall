#include "Game/PlayerClient.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AuthoritativeServer.hpp"

PlayerClient::PlayerClient()
{

}

PlayerClient::~PlayerClient()
{

}

void PlayerClient::StartUp()
{
	g_theAutoratitiveServer->AddPlayer();
	g_theAutoratitiveServer->AssignInputSystem( g_theInput );
}

void PlayerClient::BeginFrame()
{

}

void PlayerClient::EndFrame()
{
	m_game = g_theAutoratitiveServer->GetGame();
}

void PlayerClient::Render()
{
	if ( m_game != nullptr )
	{
		m_game->Render();
	}
}
