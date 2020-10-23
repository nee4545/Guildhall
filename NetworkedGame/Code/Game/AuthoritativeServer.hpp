#pragma once
#include "Game/Server.hpp"
class MultiplayerGame;
class SinglePlayerGame;
class InputSystem;

class AuthoritativeServer :public Server
{
public:


	AuthoritativeServer(  );
	~AuthoritativeServer();

public:
	void StartUp();
	void ShutDown();
	void Update( float deltaSeconds );
	void BeginFrame();
	void EndFrame();

	Game* GetGame();
	void AddPlayer();
	void AssignInputSystem(InputSystem* inputSystem);
	 
	
};


