#pragma once
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Camera;
class Texture;
class Texture;
class Game;


class StartScreen
{
public:
	StartScreen(Game* game);

	void Update( float deltaSeconds );
	void Render();
	void LoadTextures();

	Camera* m_camera = nullptr;
	
	AABB2 m_backRound1 = AABB2(Vec2(0.f,0.f),Vec2(80.f,90.f));
	AABB2 m_gameNameBox;
	AABB2 m_startBox;
	AABB2 m_createBox;
	AABB2 m_contolsBox;
	AABB2 m_quitBox;
	Texture* m_gameBg = nullptr;
	Texture* m_gameName = nullptr;
	Texture* m_startGame = nullptr;
	Texture* m_createTex = nullptr;
	Texture* m_controlsTex = nullptr;
	Texture* m_quitTex = nullptr;
	Game* m_game;

	Vec2 m_mousePosition;
	Rgba8 m_normalColor = Rgba8();
	Rgba8 m_highColor = Rgba8(100,100,100,240);


};