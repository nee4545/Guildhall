#pragma once
#include "Game/Game.hpp"
#include "Game/EntityCommon.hpp"
#include <vector>

class Camera;
class InputSystem;
class Player;
class RenderContext;
class AI;
class Bullet;
class Timer;
class Texture;

class SinglePlayerGame :public Game
{
public:
	SinglePlayerGame();
	~SinglePlayerGame();

	Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	
	void AssignInputSystem( InputSystem* inputSystem );
	void CreatePlayer();

	virtual void StartUp() override;
	virtual void Update( float deltaSeconds ) override;
	virtual void Render( ) override;

	void SpawnBullet( Vec2 position , Vec2 forwardVector , Faction faction );
	void SpawnAI();
	void DoGarbageCollection();
	void DoCollissionDetection();
	void ToggleDevConsole();

	Player* m_player;
	std::vector<AI*> m_ais;
	std::vector<Bullet*> m_bullets;
	Vec2 m_mousePosition;
	RandomNumberGenerator* m_rng = nullptr;
	Timer* m_aiSpawnTimer = nullptr;
	Texture* m_gameOverTex = nullptr;

	bool gameOver = false;
	bool devConsoleOpen = false;

};