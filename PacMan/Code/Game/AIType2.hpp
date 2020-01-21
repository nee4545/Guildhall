#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Game/AIType1.hpp"


class AIType2 :public Entity
{
public:

	AIType2( Game* game , Vec2 position );

	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Vec2 m_startingPosition;
	Vec2 m_previousPosition;
	Vec2 m_finalDestination;

	
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	void MoveUp( float deltaSeconds );
	void MoveDown( float deltaSeconds );
	void MoveLeft( float deltaSeconds );
	void MoveRight( float deltaSeconds );
	void ChangeState( States newState );
	void SetNewDestination();
	bool ValidateMove( Moves move );
	Moves GetNextMove();
	Moves GetNextMoveRandom();
	Moves nextMove = MOVE_UP;
	States m_state = STATE_CHASE;

	void DebugRender();
	void ResetVertices();
};
