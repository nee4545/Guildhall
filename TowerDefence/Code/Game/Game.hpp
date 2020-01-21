#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class Map;

class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	
public:
	
	Map* m_map=nullptr;
	
	Game();


	void Update(float deltaseconds);
	void Render();
	
	void UpdateCamera();

	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();

	Vec2 GetMousePositionScreenView();
};