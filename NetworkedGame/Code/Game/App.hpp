#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"

class Game;
class PlayerClient;

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

	static void StartMultiplayerServer( int port );
	static void ConnectToMultiplayerServer( std::string address );

	PlayerClient* m_client = nullptr;
private:
	bool m_isQuitting = false;

};
