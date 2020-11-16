#include "Game/GameUDPSocket.hpp"

void GameUDPSocket::StartSocket( int bindPort , int sendPort , std::string host /*= "127.0.0.1" */ )
{
	m_listenSocket = new UDPSocket( host , sendPort );
	m_listenSocket->Bind( bindPort );
}
