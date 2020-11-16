#pragma once
#include "Game/Client.hpp"
class Game;

class RemoteClient:public Client
{
public:
	RemoteClient();
	~RemoteClient();
	
	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Render();

	Game* m_game = nullptr;

};