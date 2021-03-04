#pragma once
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
class Texture;
class Camera;
class SpriteAnimDefTex;
class Tutorial;
class Game;

class StartScreen
{
public:
	Texture* m_backRound = nullptr;
	Texture* m_tableTex = nullptr;
	Texture* m_titleTex = nullptr;
	Texture* m_startTex = nullptr;
	Texture* m_tutorialTex = nullptr;
	Texture* m_exitTex = nullptr;

	Camera* m_camera = nullptr;
	SpriteAnimDefTex* m_heroIdleAnims = nullptr;
	SpriteAnimDefTex* m_heroAttackAnims = nullptr;

	Tutorial* m_tutorialScreen = nullptr;

	float m_time = 0.f;

	AABB2 m_table;
	AABB2 m_titleButton;
	AABB2 m_startButton;
	AABB2 m_tutorialButton;
	AABB2 m_exitButton;

	AABB2 m_heroBox1;
	AABB2 m_heroBox2;
	AABB2 m_heroBox3;

	bool hero1Idle = true;
	bool hero2Idle = true;
	bool hero3Idle = true;
	bool m_inTutorial = false;

	Rgba8 normalColor = Rgba8( 255 , 255 , 255 , 255 );
	Rgba8 highColor = Rgba8( 100 , 100 , 100 , 255 );

	Game* m_game=nullptr;

	StartScreen(Game* game);

	void Update( float deltaSeconds );
	void Render();
	void LoadTextures();
	void LoadAnimations();

};