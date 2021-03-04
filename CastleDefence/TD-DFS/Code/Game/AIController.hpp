#pragma once

class Game;
class Timer;
class Clock;

enum Decisions
{
	SPAWN_MELEE_HERO,
	SPAWN_RANGE_HERO,
	UPGRADE_MELEE_HERO,
	UPGRADE_RANGE_HERO,
	SPAWN_TOWER,
	UPGRADE_TOWER,
	SPAWN_TOWERDESTROYER,
	UPGRADE_TOWER_DESTROYER,
};

enum DecisionType
{
	OFFENSIVE,
	DEFENSIVE,
};

class AIController
{
public:
	AIController( Game* game );

	Game* m_game = nullptr;
	DecisionType m_type = OFFENSIVE;
	Decisions m_decision = SPAWN_MELEE_HERO;

	Timer* m_decisionTimer = nullptr;
	Clock* m_clock = nullptr;

	void Update( float deltaSeconds );
	void MakeDecision();
	void ExecuteDecision();

};