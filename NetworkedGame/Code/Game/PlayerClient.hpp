#pragma once
#include "Game/Client.hpp"
class RenderContext;
class InputSystem;
class Game;

class PlayerClient:public Client
{
public:
	PlayerClient();
	~PlayerClient();
	
	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Render();
	
	Game* m_game = nullptr;

};


