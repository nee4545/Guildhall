#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class BitmapFont;

class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	BitmapFont* m_font = nullptr;
public:
	

	
	Game();

	void Update(float deltaseconds);
	void Render();

	void DrawAABBDetails();
	void DrawLineDetails();
	void DrawCapsuleDetails();
	void DrawOBDBDetails();
	void DrawDiscDetails();

	Vec2 GetMousePositionScreenView();
	
	void UpdateCamera();

	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();
};