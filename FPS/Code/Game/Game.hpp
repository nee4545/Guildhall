#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/World.hpp"




class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	
public:
	
	World* world;
	Game();

	bool toggleCamera=false;
	bool paused=false;
	bool isDebugging=false;

	void Update(float deltaseconds);
	void Render();
	
	void UpdateCamera();

	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();
	

	
	





	
	
};