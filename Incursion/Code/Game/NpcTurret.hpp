#pragma once
#include "Game/Entity.hpp"
#include "Map.hpp"

class NpcTurret: public Entity
{
public:
	NpcTurret( Game* game, Vec2 position );

	Vertex_PCU m_turret_base[6];
	Vertex_PCU m_turret_base_original[6];

	Vertex_PCU m_turret_top[6];
	Vertex_PCU m_turret_top_original[6];
	
	RayCastResult m_RayCastResult;

	float cooldownTimer=1.7f;
	Vec2 m_lastSeenPosition;
	bool lastSeenSet = false;

	bool hasReachedInitial = false;
	bool hasReachedFinal = false;
	float goalDegrees;

	virtual void Update( float deltaSeconds ) override;
	void Render() override;
	virtual void Die() override;


	void DebugRender();
	void ResetVertices();
	void Shoot();
	void RotateTurret(float deltaSeconds);
};