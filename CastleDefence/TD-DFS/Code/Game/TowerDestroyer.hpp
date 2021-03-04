#pragma once
#include "Game/Entity.hpp"

class Timer;
class Clock;

class TowerDestroyer :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Timer* m_attackTimer = nullptr;
	Clock* m_attackClock = nullptr;

	float m_timeTolive = 12.f;
	float m_speed = 10.f;

	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;
	Lanes m_lane = LANE1_1;

	bool m_isWalking = false;
	bool m_isAttacking = false;
	float m_damage = 0.f;
	float m_time = 0.f;

	TowerDestroyer( Game* game , Vec2 position , Faction faction = FACTION_GOOD,Upgrade upgrade= UPGRADE_NONE, Lanes lane=LANE1_1 );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	
	void PopulateStatsBasedOnUpgrade();
	void DebugRender();
	void ResetVertices();
	Texture* GetTextureBasedOnStatus();
	Entity* GetTowerToAttack();
};