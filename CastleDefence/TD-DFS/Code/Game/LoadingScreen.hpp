#pragma once

class Game;
class Texture;
class Camera;
#include "Engine/Core/AABB2.hpp"

class LoadingScreen
{
public:
	Game* m_game=nullptr;

	LoadingScreen( Game* game );
	~LoadingScreen();
	void Update();
	void Render();
	void LoadTextures();

	Camera* m_camera = nullptr;

	AABB2 m_backround;
	AABB2 m_table;
	AABB2 m_Loading;
	AABB2 m_sprite1;
	AABB2 m_sprite2;

	Texture* m_backroundTex;
	Texture* m_tableTex;
	Texture* m_LoadingTex;
	Texture* m_sprite1Tex;
	Texture* m_sprite2Tex;
	
};