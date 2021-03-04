#pragma once
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Texture;
class Camera;
class SpriteAnimDefTex;
class Game;
class StartScreen;
class BitmapFont;

class Tutorial
{
public:
	
	float m_time = 0.f;
	StartScreen* m_startScreen = nullptr;

	AABB2 m_table;
	AABB2 m_playButton;
	AABB2 m_exitButton;

	AABB2 m_rules;
	AABB2 m_rulesText;
	AABB2 m_controls;
	AABB2 m_controlsText;

	AABB2 m_heroSpriteBox;
	AABB2 m_heroSprite;
	AABB2 m_heroCostBox;
	AABB2 m_upgradeBox;
	AABB2 m_upgradeCostBox;

	AABB2 m_heroCostText;
	AABB2 m_upgradeCostText;

	AABB2 m_costArrow;
	AABB2 m_upgrdeCostArrow;
	AABB2 m_heroArrow;
	AABB2 m_upgradeArrow;
	AABB2 m_heroButton;
	AABB2 m_upgradeButton;

	AABB2 m_hudInfo;

	AABB2 m_heroBox1;
	AABB2 m_heroBox2;
	AABB2 m_startButton;
	AABB2 m_mainMenuButton;

	SpriteAnimDefTex* m_heroIdleAnims = nullptr;
	SpriteAnimDefTex* m_heroAttackAnims = nullptr;
	
	BitmapFont* m_font = nullptr;

	Texture* m_tableTex = nullptr;
	Texture* m_rulesTex = nullptr;
	Texture* m_rulesTextTex = nullptr;
	Texture* m_controlsTex = nullptr;
	Texture* m_controlsTextTex = nullptr;

	Texture* m_heroSpriteBoxTex = nullptr;
	Texture* m_heroSpriteTex = nullptr;
	Texture* m_costBoxTex = nullptr;
	Texture* m_upgradeBoxTex = nullptr;
	Texture* m_rightArrowTex = nullptr;
	Texture* m_heroCostTextTex = nullptr;
	Texture* m_upArrowTex1 = nullptr;
	Texture* m_upArrowTex2 = nullptr;
	Texture* m_upgradeCostTextTex = nullptr;
	Texture* m_heroButtonTex = nullptr;
	Texture* m_upgradeButtonTex = nullptr;
	Texture* m_HudInfoTex = nullptr;
	Texture* m_startTex = nullptr;
	Texture* m_MainMenuTex = nullptr;

	Game* m_game = nullptr;


	Tutorial( Game* game, StartScreen* startScreen );

	void Update( float deltaSeconds );
	void Render();
	void LoadTextures();

	bool heroIdle = true;
	bool hero1Idle = true;
};