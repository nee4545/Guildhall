#pragma once
#include "Engine/Core/UDPSocket.hpp"
#include "Engine/Core/SynchronizedLockFreeQueue.hpp"
#include <thread>

class GameUDPSocket
{
public:
	GameUDPSocket();
	~GameUDPSocket();

	UDPSocket* m_listenSocket = nullptr;
	void StartSocket( int bindPort , int sendPort , std::string host = "127.0.0.1" );
	void AddMessage( std::string& message );
	void Close();

	void Reader( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& readArray );
	void Writer( UDPSocket& socket , SynchronizedLockFreeQueue<std::string>& writeArray );

	SynchronizedLockFreeQueue<std::string> m_readArray;
	SynchronizedLockFreeQueue < std::string > m_wirteArray;

	std::thread* readerThread = nullptr;
	std::thread* writerThread = nullptr;
};