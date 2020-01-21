#pragma once
#include "Game/Entity.hpp"
#include "Game/Map.hpp"

class NpcTank : public Entity
{
public:
	NpcTank( Game* game, Vec2 position );
	

	virtual void Update( float deltaSeconds ) override;
	void Render() override;
	virtual void Die() override;

	Vertex_PCU m_tank[6];
	Vertex_PCU m_turret[6];

	Vertex_PCU m_tank_original[6];
	Vertex_PCU m_turret_original[6];

	float gunCoolDownTimer = 1.7f;
	float directionChangeTimer = 2.0f;
	float m_turretAngle=0.f;
	float direction=0.f;

	Vec2 m_lastSeenPosition;
	bool lastSeenSet = false;

	RayCastResult ray1;
	RayCastResult ray2;
	RayCastResult ray3;

	Vec2 positionBeforeASecond;
	float positionTimer =1.f;

	void ResetVertices();
	void DebugRender();
	void RotateTank(float angleDegrees, float deltaSeconds);
	void MoveTank(float deltaSeconds);
	void Shoot();
	



};