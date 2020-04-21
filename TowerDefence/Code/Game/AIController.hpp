#pragma once
#include "Engine/Math/Vec2.hpp"

class Timer;
class Map;
class RandomNumberGenerator;

enum Decisions
{
	SPAWN_HERO,
	SPAWN_SPIKE,
	SPAWN_DINO,
	UPGRADE_HERO,
	UPGRADE_DINO,
	UPGRADE_SPIKE,
	DO_NOTHING
};

struct Decision
{
	Decisions d;
	float xPosition;
	int laneNumber=0;
};

class AIController
{

public:
	AIController();
	void TakeDefensiveDecision();
	void TakeOffensiveDecision();
	void TakeStupidDecision();
	void TakeDecision();
	void Execute();
	void Update();

	Map* m_map = nullptr;
	Decision m_decision;
	Timer* m_timer = nullptr;
	RandomNumberGenerator* m_rng = nullptr;

};