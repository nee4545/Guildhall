#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/EntityCommon.hpp"

class SpriteAnimDefinition;
class SpriteAnimDefTex;
class Game;
class Clock;
class Timer;
class OccupancyMap;

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
	void UpdateAndClearSingleOccupancyMap();
	void CheckPlayerInSight();
	void UpdateBehavior();
	void ExecuteIdleBehavior();
	void ExecuteChaseBehavior();
	void LoadAnimations();
	void SetDestinationForSharedAI(OccupancyMap* sharedMap);
	void Move( float deltaSeconds ,float speed ,Vec2 dirVec );

public:
	Vertex_PCU m_vertices[ 6 ];
	

	Game* m_game = nullptr;

	Clock* m_animClock = nullptr;
	Timer* m_animTimer = nullptr;

	Timer* m_occupancyPropogateTimer = nullptr;
	Timer* m_occupancyUpdateTimer = nullptr;
	int tileIndex = -1;

	float m_time = 0.f;
	OCCAI_States m_state = OCC_IDLE;
	OCCAI_Type m_type = OCC_AI_SINGLE_MAP;

	MovingHelper* m_helper = nullptr;

	float m_visionRadius = 15.f;

	Vec2 m_lastSeenPosition;
	Vec2 m_nextMovePos;
	bool m_lastSeenSet = false;

	IntVec2 m_currentMaxCoords;

	OccupancyMap* m_occupancyMap = nullptr;
	bool initialUpdateDone = false;
	bool m_mapCleared = false;
	bool m_destinationSet = false;

	SpriteAnimDefTex* m_anims = nullptr;
	Vec2 m_maxUV = Vec2( 1.f , 1.f );
	Vec2 m_minUV = Vec2( 0.f , 0.f );


};