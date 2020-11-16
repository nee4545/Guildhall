#pragma once
#include "Game/Game.hpp"
#include "Game/EntityCommon.hpp"
#include "Engine/Core/Timer.hpp"
#include <string>

class Camera;
class Player;
class AI;
class Bullet;
class Timer;
class RandomNumberGenerator;

class MultiplayerGame : public Game
{
public:
	MultiplayerGame();
	~MultiplayerGame();

	static int entityID;

	Camera* m_camera = nullptr;

	void AssignInputSystemForPlayer1( InputSystem* inputSystem );
	void CreatePlayer();
	void CreateSecondPlayer();

	void CreateOrUpdateEntitiesFromStr( std::string messageStr );
	void SpawnBullet( Vec2 position , Vec2 forwardVector , Faction faction );
	void Update( float deltaSeconds );
	void DoCollissionDetection();
	void DoGarbageCollection();
	void Render();
	void SpawnAI();

	Player* m_player1 = nullptr;
	Player* m_player2 = nullptr;
	Vec2 m_mousePosition;
	InputSystem* m_player1Input = nullptr;
	InputSystem* m_player2Input = nullptr;

	std::vector<AI*> m_ais;
	std::vector<Bullet*> m_bullets;
	RandomNumberGenerator* m_rng = nullptr;
	int entityId = 3;

	Timer* aiSpawnTimer = nullptr;
	bool aiSpawnned = false;
};