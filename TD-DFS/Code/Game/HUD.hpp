#pragma once

#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
class Game;
class Texture;
class Timer;
class Clock;
class SpriteAnimDefTex;

class HUD
{
public:
	friend class Game;

	Game* m_game;

	AABB2 HeroSpriteBox;
	AABB2 HeroCostBox;
	AABB2 HeroUpgradeBox;
	AABB2 HeroUpgradeCostBox;
	AABB2 HeroUpgradeInfoBox;

	Texture* SpriteBoxTexure =nullptr;
	Texture* CostBoxTexture =nullptr;
	Texture* UpgradeBoxTexture = nullptr;
	SpriteAnimDefTex* heroUpgrade0Anims = nullptr;

	Timer* UpgradeUpTimer = nullptr;
	Timer* UpgradeDownTimer = nullptr;
	Clock* UpgradeUpClock = nullptr;
	Clock* DownClock = nullptr;

	Rgba8 NormalColor = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 HighLightedColor = Rgba8( 200 , 200 , 200 , 255 );

	float m_time = 0.f;
	bool isUpdateInfoBoxClosed = true;


public:
	HUD( Game* game );

	void Update( float deltaSeconds );
	void Render();
	void LoadTextures();

};