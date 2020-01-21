#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

enum GameStates
{
	STATE_START=0,
	STATE_PLAYING,
	STATE_WIN,
	STATE_GAMEOVER
};

class Map;

class Game
{
	RandomNumberGenerator rng;
	Camera* m_UICamera = nullptr;
	
public:
	
	Camera* m_camera=nullptr;
	int currentLives = 3;
	int points = 0;
	GameStates m_state = STATE_START;
	Map* m_map = nullptr;
	BitmapFont* m_BitmapFont = nullptr;
	Game();

	void Update(float deltaseconds);
	void Render();
	
	void UpdateCamera();
	void ChangeState( GameStates newState );
	void DisplayStartScreen();
	void RenderUI();
	void HandleStates();
	void DisplayGameOverScreen();
	void DisplayWinScreen();
	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();
};