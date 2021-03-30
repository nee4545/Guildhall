#pragma once
#include "Game/Entity.hpp"
#include "Game/InfluenceMap.hpp"

class SpriteAnimDefinition;
class Game;
class Clock;
class Timer;
class OccupancyMap;

enum AI_STATES
{
	IDLE,
	MOVING,
	MELEE_ATTACK,
	RANGE_ATTACK_1,
	RANGE_ATTACK_2,
	DEAD,
};



enum AI_TYPE
{
	TYPE_1, // use A*, not affected by influence map, potential fields, no occupancy maps
	TYPE_2, // use A*, affected by influence maps , potential fields , no occupancy maps
	TYPE_3, // use A*, affeccted by influence maps, potential fields and occupancy maps
	TYPE_4, // use Dijkstra, not affected by influence map, potential fields, no occupancy maps
	TYPE_5, // use DIjkstra, affected by influence maps, potential fields, no occupancy maps
	TYPE_6, // use Dijkstra, affected by influence maps, potential fields and occupancy maps
	TYPE_7, // A* without influence maps and potential fields, use occupancy maps
	TYPE_8, // Dijkstra's without influence maps and potential fields, use occupancy maps
	
};

enum AI_Behavior
{
	AI_IDLE,
	PATROL,
	CHASE_PLAYER,
	INVETIGATE_SOUND,
	SHOOT_PLAYER,
	SEARCH_OCCUPANCY_MAP,
};






class MonsterAI :public Entity
{

public:

	MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims );
	MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims, SpriteAnimDefinition* rangeAttack1Anims );
	MonsterAI( Game* game , AI_TYPE type , SpriteAnimDefinition* idleAnims , SpriteAnimDefinition* walkAnims , SpriteAnimDefinition* meleeAttackAnims, SpriteAnimDefinition* rangeAttack1Anims, SpriteAnimDefinition* rangeAttack2Anims );
	~MonsterAI();

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void DebugRender() override;
	virtual void Die() override; 
	void SetVerticesBasedOnAspect( Vec2 aspect );

	void ChangeBehavior( AI_Behavior newBehavior );
	void CheckPlayerInSight();
	Vec2 GetPatrolPoint();
	void UpdateAndClearSingleOccupancyMap();

	void FindPathToRandomTile();
	void ChangeBehavior();
	void GetPath();

public:

	Vertex_PCU m_vertices[ 6 ];
	Vec2 m_minUV;
	Vec2 m_maxUV;

	Game* m_game = nullptr;

	Vec2 m_aspectAtWalk;
	Vec2 m_aspectAtIdle;
	Vec2 m_aspectAtMeleeAttack;
	Vec2 m_aspectAtRangeAttack1;
	Vec2 m_aspectAtRangeAttack2;
	Vec2 m_aspectAtDeath;

	AI_STATES m_currentState = MOVING;
	AI_TYPE m_currentType = TYPE_1;
	AI_Behavior m_currentBehavior = PATROL;
	AI_Behavior m_previousBehavior = PATROL;

	SpriteAnimDefinition* m_animationAtIdle = nullptr;
	SpriteAnimDefinition* m_animationAtWalk = nullptr;
	SpriteAnimDefinition* m_animationAtMeleeAttack = nullptr;
	SpriteAnimDefinition* m_animationAtAttack1 = nullptr;
	SpriteAnimDefinition* m_animationAtAttack2 = nullptr;

	Clock* m_animClock = nullptr;
	Timer* m_animTimer = nullptr;
	Timer* m_rotation1Timer = nullptr;
	Timer* m_rotation2Timer = nullptr;
	Timer* m_occMovetimer = nullptr;

	Timer* m_occupancyPropogateTimer = nullptr;
	Timer* m_occupancyUpdateTimer = nullptr;

	Timer* m_sectorRotationTimer = nullptr;

	bool m_hasRangeAttack = false;
	bool m_hasMultipleRangeAttack = false;

	float m_time = 0.f;
	float m_senseRadius = 3.f;

	MovingHelper* m_helper = nullptr;

	Vec2 m_nextMovePosition;

	Vec2 m_forwardVec = Vec2(0.f,-1.f);

	int tileIndex = -1;
	Vec2 moveVec;
	Vec2 m_lastSeenPosition;
	bool m_lastSeenSet = false;
	bool clearDone = false;

	bool roationTimerStarted = false;
	bool initialClearDone = false;

	OccupancyMap* m_occupancyMap = nullptr;
	//OccupancyMap* m_occMap = nullptr;

	std::vector<Vec2> m_patrolPoints;
	float m_soundRadius = 10.f;
	float m_shootingRadius = 35.f;
	float m_visionRadius = 20.f;

	int m_currentPatrolIndex = 0;
	bool m_hasPatrolPoint = false;

	bool m_moveWithoutPathfinding = false;
	float directionMultiplier = 1.f;

	IntVec2 m_currentMoveCoords;

};