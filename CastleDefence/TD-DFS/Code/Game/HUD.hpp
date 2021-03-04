#pragma once

#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
class Game;
class Texture;
class Timer;
class Clock;
class SpriteAnimDefTex;

enum HUDType
{
	TYPE_MELEE,
	TYPE_RANGE,
	TYPE_TOWER,
	TYPE_TOWER_DESTROYER,
};

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
	Texture* m_spriteTex = nullptr;
	SpriteAnimDefTex* heroUpgrade0Anims = nullptr;
	SpriteAnimDefTex* heroUpgrade1Anims = nullptr;
	SpriteAnimDefTex* heroUpgrade2Anims = nullptr;
	SpriteAnimDefTex* heroUpgrade3Anims = nullptr;

	Timer* UpgradeUpTimer = nullptr;
	Timer* UpgradeDownTimer = nullptr;
	Clock* UpgradeUpClock = nullptr;
	Clock* DownClock = nullptr;

	Rgba8 NormalColor = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 HighLightedColor = Rgba8( 200 , 200 , 200 , 255 );

	float m_time = 0.f;
	bool isUpdateInfoBoxClosed = true;
	HUDType m_type = TYPE_MELEE;


public:
	HUD( Game* game,HUDType type );

	void Update( float deltaSeconds );
	void Render();
	void RenderCoins();
	void GetTextureForType();
	void LoadTextures();

};