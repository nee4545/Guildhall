#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Timer;
class Clock;

class Tower :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	AABB2 m_towerBase;
	AABB2 m_tower;

	float m_health = 20.f;

	Timer* m_attackTimer =nullptr;
	Clock* m_attackClock =nullptr;

	Timer* m_rotationTimer = nullptr;

	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;

	bool rotateClockWise = false;

	Tower( Game* game , Vec2 position , Faction faction = FACTION_GOOD, Upgrade upg = UPGRADE_NONE );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	virtual void TakeDamage( float damageAmt ) override;

	Texture* m_healbarBase = nullptr;
	Texture* m_healthbar = nullptr;

	void DebugRender();
	void ResetVertices();

	void PopulateStats();

	void UpgradeTower(Upgrade u);
};