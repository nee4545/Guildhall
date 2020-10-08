#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>
#include <array>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Network/NetworkDefs.hpp"
#include "../Core/EventSystem.hpp"
#include "Engine/UDPListner.hpp"

extern InputSystem* g_theInput;
extern NetworkSystem* g_theNetwork;

NetworkSystem::NetworkSystem()
{
	g_theEventSystem.SubscribeToEvent( "StartServer" , StartServer );
	g_theEventSystem.SubscribeToEvent( "Connect" , Connect );
	g_theEventSystem.SubscribeToEvent( "SendServerMessage" , SendServerMessage );
	g_theEventSystem.SubscribeToEvent( "SendClientMessage" , SendClientMessage );
	g_theEventSystem.SubscribeToEvent( "StopServer" , StopServ );
	g_theEventSystem.SubscribeToEvent( "Disconnect" , Disconnect );
	g_theEventSystem.SubscribeToEvent( "OpenUDPPort" , OpenUDPPort );
	g_theEventSystem.SubscribeToEvent( "SendUDPMessage" , SendUDPMessage );
	g_theEventSystem.SubscribeToEvent( "CloseUDPPort" , CloseUDPPort );
	m_UDPlistner = new UDPListner();
}

NetworkSystem::~NetworkSystem()
{
	delete m_UDPlistner;
}

void NetworkSystem::StartUp()
{
	WSADATA wsaData;
	WORD wVersion MAKEWORD( 2 , 2 );
	int iResult = WSAStartup( wVersion , &wsaData );

	if ( iResult != 0 )
	{
		//error
	}

	m_clientSocket = INVALID_SOCKET;
	

}

void NetworkSystem::ShutDown()
{
	int iResult = WSACleanup();

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}
}

void NetworkSystem::BeginFrame()
{
	if ( m_isListening )
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
						g_theConsole.PrintString( Rgba8() , "Client connected :" + GetAddress() );
					}
				}

				if ( m_clientSocket != INVALID_SOCKET )
				{
					m_server->Receive( m_clientSocket );
				}
			}
		}

		if ( m_client != nullptr )
		{
			if ( m_client->IsValid()  )
			{
				m_client->Receive( m_client->m_socket );
			}
		}

	}
}

void NetworkSystem::EndFrame()
{

}

std::string NetworkSystem::GetAddress()
{
	std::array<char , 128> addressStr;

	sockaddr clientAddr;
	int addrSize = sizeof( clientAddr );

	int iResult = getpeername( m_clientSocket , &clientAddr , &addrSize );

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}

	DWORD outlen = (DWORD)addressStr.size();
	iResult = WSAAddressToStringA( &clientAddr , addrSize , NULL , &addressStr[ 0 ] , &outlen );

	if ( iResult == SOCKET_ERROR )
	{
		//error
	}

	return &addressStr[0];
}

void NetworkSystem::CreateTCPServer(int portNum)
{
	m_server = new TCPServer( portNum );

	m_server->StartServer();
	m_server->Bind();
	m_server->Listen();
}

void NetworkSystem::CreateTCPClient(std::string address)
{
	m_client = new TCPClient();
	m_client->m_socket = m_client->Connect( address , 48000 );
}

void NetworkSystem::SendMessageFromServer( std::string message )
{
	if ( m_server == nullptr )
	{
		g_theConsole.PrintError( "No TCP server created" );
	}
	else if ( !m_server->IsValid() )
	{
		g_theConsole.PrintError( "Server not created" );
	}
	else
	{
		std::array<char , 256> buffer;
		MessageHeader* header = reinterpret_cast< MessageHeader* >( &buffer[ 0 ] );
		header->m_id = 1;
		header->m_size = (uint16_t)message.length();

		for ( int i = 0; i < message.length(); i++ )
		{
			buffer[ i + 4 ] = message[ i ];
		}

		m_server->Send( &buffer[0] , (int)buffer.size() ,m_clientSocket );
	}
	
}

void NetworkSystem::SendMessageFromClient( std::string message )
{
	if ( m_client == nullptr )
	{
		g_theConsole.PrintError( "No TCP client created" );
	}
	else if ( !m_client->IsValid() )
	{
		g_theConsole.PrintError( "client socket not created" );
	}
	else
	{
		std::array<char , 256> buffer;
		MessageHeader* header = reinterpret_cast< MessageHeader* >( &buffer[ 0 ] );
		header->m_id = 1;
		header->m_size = (uint16_t)message.length();

		for ( int i = 0; i < message.length(); i++ )
		{
			buffer[ i + 4 ] = message[ i ];
		}

		m_client->Send( &buffer[0] ,(int)buffer.size(), m_client->m_socket );
		
	}
}

bool NetworkSystem::StartServer( EventArgs& args )
{
	int portNum = args.GetValue( "portNum", 48000 );

	g_theNetwork->CreateTCPServer(portNum);

	return true;
}

bool NetworkSystem::Connect( EventArgs& args )
{
	std::string address = args.GetValue( "address" , "" );
	g_theNetwork->CreateTCPClient(address);

	return false;
}

bool NetworkSystem::SendServerMessage( EventArgs& args )
{
	std::string message = args.GetValue( "msg" , "" );
	g_theNetwork->SendMessageFromServer(message);
	return true;
}

bool NetworkSystem::SendClientMessage( EventArgs& args )
{
	std::string message = args.GetValue( "msg" , "" );
	g_theNetwork->SendMessageFromClient( message );
	return true;
}

bool NetworkSystem::StopServ( EventArgs& args )
{
	UNUSED( args );
	g_theNetwork->StopServer();
	return false;
}

bool NetworkSystem::Disconnect( EventArgs& args )
{
	UNUSED( args );
	g_theNetwork->StopClient();
	return false;
}

bool NetworkSystem::OpenUDPPort( EventArgs& args )
{
	std::string bindPort = args.GetValue( "bindPort" , "48000" );
	std::string sendPort = args.GetValue( "sendPort" , "48001" );
	std::string host = args.GetValue( "host" , "127.0.0.1" );

	g_theNetwork->StartUDPListner( atoi(bindPort.c_str()),  atoi(sendPort.c_str()) , host );
	return true;

}

void NetworkSystem::StopServer()
{
	SendMessageFromServer( "Server Disconnecting" );

	if ( m_server->m_listenPort != INVALID_SOCKET )
	{
		closesocket( m_server->m_listenSocket );
	}

	if ( m_clientSocket != INVALID_SOCKET )
	{
		closesocket( m_clientSocket );
	}

	if ( m_server != nullptr )
	{
		delete m_server;
		m_server = nullptr;
	}

}

void NetworkSystem::StopClient()
{
	SendMessageFromClient( "Client Disconnecting" );

	if ( m_client->m_socket != INVALID_SOCKET )
	{
		closesocket( m_client->m_socket );
	}

	if ( m_client != nullptr )
	{
		delete m_client;
		m_client = nullptr;
	}
}

void NetworkSystem::StartUDPListner( int bindPort , int sendPort , std::string host /*= "127.0.0.1"*/ )
{
	m_UDPlistner->StartSocket( bindPort , sendPort , host );
}

void NetworkSystem::SendUDPMessage( std::string message )
{
	m_UDPlistner->AddMessage( message );
}

bool NetworkSystem::SendUDPMessage( EventArgs& args )
{
	std::string msg = args.GetValue( "msg" , "" );
	g_theNetwork->SendUDPMessage( msg );

	return true;
}

bool NetworkSystem::CloseUDPPort( EventArgs& args )
{
	UNUSED( args );
	g_theNetwork->m_UDPlistner->Close();
	return true;
}

