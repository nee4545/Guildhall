#pragma once
#include "Game/Entity.hpp"

class Texture;
class Timer;

class UncontrolledHero :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	float m_time = 0.f;
	float m_speed = 5.f;
	float m_attackSpeed = 1.f;
	float m_damage = 1.f;
	float m_animationSpeed = 1.f;
	float m_totalHealth = 0.f;

	bool isSpecial = false;

	Timer* m_attackTimer = nullptr;

	bool isAttacking = true;
	bool isRunning = false;

	Entity* m_heroToAttack = nullptr;

	Texture* m_healbarBase = nullptr;
	Texture* m_healthbar = nullptr;

	Faction m_faction = FACTION_GOOD;
	Lanes m_lane = LANE1_1;

	UncontrolledHero( Game* game , Vec2 position , Faction faction = FACTION_GOOD , Lanes lane = LANE1_1, bool isUp = false  );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	virtual void TakeDamage( float damageAmt ) override;

	void DebugRender();
	void ResetVertices();

	Texture* GetTextureBasedOnCurrentState();
	void PopulateHeroStatesBasedOnUpgrade();
	void GetHeroToAttack();

};
