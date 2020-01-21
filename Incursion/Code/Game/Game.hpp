#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/World.hpp"

enum SoundIds
{
	PLAYER_SHOOT,
	PLAYER_HIT,
	PLAYER_DIED,
	ENEMY_HIT,
	ENEMY_SHOOT,
	ENEMY_DIED,
	PLANE_SOUND

};

enum GameState
{
	LOADING,
	ATTRACT,
	PLAYING,
	VICTORY,
	GAME_OVER,
	PAUSED
};



class Game
{

	RandomNumberGenerator rng;
	BitmapFont* m_BitmapFont = nullptr;
	GameState m_state = LOADING;
	
	int frameNumber = 0;
	
public:
	
	World* world;
	Game();

	bool toggleCamera=false;
	bool paused=false;
	bool isDebugging=false;
	int currentLives = 3;
	int airStrikeCharges = 3;
	bool didPlayerWin =false;

	bool isInAttractScreenMode= true;
	bool isInGameOverScreen =false;
	bool isInGameWinScreen = false;
	bool isInStartscreen = false;
	bool assetsLoaded = false;

	Camera* m_camera=nullptr;
	Camera* m_UICamera= nullptr;

	void Update(float deltaseconds);
	void Render();
	
	void UpdateCamera();

	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();

	void LoadTextures();
	void LoadSounds();
	void LoadFonts();
	void RenderUI();
	void RenderStartScreen();
	void RenderGameOverScreen();
	void RenderWinScreen();
	void HandleRenderScreens();
	void RenderLoadingScreen();
	void RenderPausedScreen();
	void RenderPlayingScreen();
	void RenderDeadScreen();

};