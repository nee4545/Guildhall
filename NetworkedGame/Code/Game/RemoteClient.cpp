#pragma once
#include "Game/RemoteClient.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AuthoritativeServer.hpp"
#include "Game/RemoteServer.hpp"

RemoteClient::RemoteClient()
{

}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::StartUp()
{

}

void RemoteClient::BeginFrame()
{

}

void RemoteClient::EndFrame()
{
	m_game = g_theRemoteServer->GetGame();
}

void RemoteClient::Render()
{
	if ( m_game != nullptr )
	{
		m_game->Render();
	}
}
