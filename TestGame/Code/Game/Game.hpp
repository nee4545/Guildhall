#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"




class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	
public:
	
	
	Game();

	void Update(float deltaseconds);
	void Render();
	
	void UpdateCamera();

	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();
};