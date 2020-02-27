#pragma once
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

	
private:

	bool m_isQuitting=false;
	
	Game* thegame=nullptr;

	
};
