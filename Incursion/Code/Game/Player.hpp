#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Player:public Entity
{
public:

	Player(Game* game,Vec2 position);

	Vertex_PCU m_tank[6];
	Vertex_PCU m_turret[6];
	Vertex_PCU m_crosshair[6];

	Vertex_PCU m_tank_original[6];
	Vertex_PCU m_turret_original[6];
	Vertex_PCU m_crosshair_original[6];

	float m_turretAngle=0.f;
	float m_speed=PLAYER_TANK_SPEED;
	Vec2 m_crosshairPosition;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() override;
	virtual void Die() override;

	void MoveTank(float deltaSeconds);
	void RotateTurret(float deltaSeconds);
	void RotateTank(float deltaSeconds);

	void UpdateFromJoystick(float deltaSeconds);
	void ShootBullet();

	void DebugRender();
	void ResetTank();
	void ResetTurret();
	void RespawnTank();
	void ResestCrosshair();
};
