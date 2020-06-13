#pragma once

#include "Game/Entity.hpp"
class Texture;
class Timer;

class RangeHero :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	float m_time = 0.f;
	float m_speed = 5.f;
	float m_attackSpeed = 1.f;
	float m_damage = 1.f;
	float m_animationSpeed = 1.f;

	Timer* m_attackTimer = nullptr;

	bool isAttacking = false;
	bool isRunning = true;


	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;

	RangeHero( Game* game , Vec2 position , Faction faction = FACTION_GOOD );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void TakeDamage( float damageAmt );

	void DebugRender();
	void ResetVertices();
	Texture* GetTextureBasedOnCurrentState();

	void PopulateHeroStatesBasedOnUpgrade();
	void Attack();
	void DetectHeroToAttack();
	void DetectPlayerActions();
};