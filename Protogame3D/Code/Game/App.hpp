#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

class Game;

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

	bool HandleQuitRequested();

	void ResetGame();

	int GetFPS();

	
private:

	bool m_isQuitting=false;
	
	//Game* thegame=nullptr;

	
};
