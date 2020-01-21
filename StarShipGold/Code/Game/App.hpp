#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

class Game;
class Camera;

class App
{
public:
	App()
	{
		
	};
	~App();

	void Startup();
	void Shutdown();
	void RunFrame();

	bool isQuitting() const
	{
		return m_isQuitting;
	}

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render() const;

	void EndFrame();

	void UpdateShip( float deltaSeconds );
	void RenderShip() const;

	bool HandelKeyPressed(unsigned char keyCode);
	bool HandelKeyReleased(unsigned char keyCode);
	bool HandleQuitRequested();

	void ResetGame();

	bool upPressed=false;
	bool leftPressed=false;
	bool rightPressed=false;
	bool spacePressed=false;
	
	bool oPressed=false;
	bool f1Presssed=false;
	
	Camera* ui_cam = nullptr;
	Camera* game_cam=nullptr;

private:

	bool m_isQuitting=false;
	bool m_isPaused=false;
	bool m_isSlowMo=false;


	Game* thegame=nullptr;

	
};
