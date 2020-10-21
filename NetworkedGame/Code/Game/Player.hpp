#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Player:public Entity
{
public:

	Player(Game* game,Vec2 position);

	Vertex_PCU m_tank[6];
	Vertex_PCU m_turret[6];

	Vertex_PCU m_tank_original[6];
	Vertex_PCU m_turret_original[6];

	float m_turretAngle=0.f;
	float m_speed=PLAYER_TANK_SPEED;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() override;
	virtual void Die() override;
};
