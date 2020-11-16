#pragma once
#include "Game/Game.hpp"
#include <string>

class Camera;
class Player;
class AI;
class Bullet;

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

	void Update( float deltaSeconds );
	void Render();

	Player* m_player1 = nullptr;
	Player* m_player2 = nullptr;

	InputSystem* m_player1Input = nullptr;
	InputSystem* m_player2Input = nullptr;

	std::vector<AI*> m_ais;
	std::vector<Bullet*> m_bullets;
};