#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

enum Moves
{
	MOVE_UP=0,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT
};

enum States
{
	STATE_CHASE = 0 ,
	STATE_FLEE ,
	STATE_SLEEP
};

class AIType1 :public Entity
{
public:

	AIType1( Game* game , Vec2 position );

	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Vec2 m_startingPosition;
	Vec2 m_finalDestination;
	bool m_hasReachedDestination = false;
	Vec2 m_previousPosition;

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	void MoveUp( float deltaSeconds );
	void MoveDown( float deltaSeconds );
	void MoveLeft( float deltaSeconds );
	void MoveRight( float deltaSeconds );
	void SetNewDestination();
	void ChangeState( States newState );
	Moves GetNextMove();
	Moves nextMove = MOVE_UP;
	States m_state = STATE_CHASE;

	void DebugRender();
	void ResetVertices();
};