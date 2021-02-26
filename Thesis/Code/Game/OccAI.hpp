#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/IntVec2.hpp"

class SpriteAnimDefinition;
class Game;
class Clock;
class Timer;
class OccupancyMap;

enum OCCAI_States
{
	OCC_IDLE,
	OCC_CHASE_PLAYER,
	OCC_SEARCH_MAP,
};

enum OCCAI_Type
{
	OCC_AI_SINGLE_MAP ,
	OCC_AI_SHARED_MAP ,
};


class OccAI :public Entity
{
public:
	OccAI();
	~OccAI();

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void DebugRender() override;
	virtual void Die() override;

	void SetVerticesBasedOnAspect( Vec2 aspect );
	
	void CheckPlayerInSight();
	void UpdateBehavior();

public:
	Vertex_PCU m_vertices[ 6 ];
	Vec2 m_minUV;
	Vec2 m_maxUV;

	Game* m_game = nullptr;

	Clock* m_animClock = nullptr;
	Timer* m_animTimer = nullptr;

	Timer* m_occupancyUpdateTimer = nullptr;
	int tileIndex = -1;

	float m_time = 0.f;
	OCCAI_States m_state = OCC_IDLE;
	OCCAI_Type m_type = OCC_AI_SINGLE_MAP;

	MovingHelper* m_helper = nullptr;

	Vec2 m_lastSeenPosition;
	Vec2 m_nextMovePos;
	bool m_lastSeenSet = false;

	IntVec2 m_currentMaxCoords;

	OccupancyMap* m_occupancyMap = nullptr;
	bool initialUpdateDone = false;
	bool m_mapCleared = false;

};