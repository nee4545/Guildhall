#pragma once

//class SOCKET;

class NetworkSystem
{
public:
	NetworkSystem();
	~NetworkSystem();

	void StartUp();
	void ShutDown();
	void BeginFrame();
	void EndFrame();

	int m_listenPort = -1;
	bool m_isListening = false;
	unsigned long long m_listenSocket;

};