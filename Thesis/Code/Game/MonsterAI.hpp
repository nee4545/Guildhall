#pragma once
#include "Game/Entity.hpp"

class SpriteAnimDefinition;
class Game;
class Clock;
class Timer;

enum AI_STATES
{
	IDLE,
	MOVING,
	MELEE_ATTACK,
	RANGE_ATTACK_1,
	RANGE_ATTACK_2,
	DEAD,
};

enum Moves
{
	MOVE_UP ,
	MOVE_DOWN ,
	MOVE_RIGHT ,
	MOVE_LEFT ,
	MOVE_TOPRIGHT ,
	MOVE_TOPLEFT ,
	MOVE_BOTLEFT ,
	MOVE_BOTRIGHT ,
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
	WANDER,
	CHASE_PLAYER,
};



struct MovingHelper
{
	Moves move;
	std::vector<int> path;
	int currentIndex = 0;

	int GetNextMove()
	{
		if ( currentIndex <0 )
		{
			Reset();
			return -1;
		}
		return path[ currentIndex-- ];
	}

	void Reset()
	{
		currentIndex = 0;
		path.clear();
	}

	bool DoesHavePath()
	{
		return path.size() > 0;
	}

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

	void FindPathToRandomTile();

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
	AI_Behavior m_currentBehavior = WANDER;

	SpriteAnimDefinition* m_animationAtIdle = nullptr;
	SpriteAnimDefinition* m_animationAtWalk = nullptr;
	SpriteAnimDefinition* m_animationAtMeleeAttack = nullptr;
	SpriteAnimDefinition* m_animationAtAttack1 = nullptr;
	SpriteAnimDefinition* m_animationAtAttack2 = nullptr;

	Clock* m_animClock = nullptr;
	Timer* m_animTimer = nullptr;

	bool m_hasRangeAttack = false;
	bool m_hasMultipleRangeAttack = false;

	float m_time = 0.f;

	MovingHelper* m_helper = nullptr;

	Vec2 m_nextMovePosition;

	Vec2 m_forwardVec = Vec2(0.f,-1.f);

	int tileIndex = -1;
	Vec2 moveVec;
};