#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Timer.hpp"


class SpriteAnimDefTex;
class Map;
class Texture;

enum Upgrade
{
	UPGRADE_NONE=0,
	UPGRADE_HEALTH,
	UPGRADE_SPEED,
	UPGRADE_ULTIMATE,
};

class Hero :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;

	float m_health=0.f;
	float m_speed = 0.f;
	bool m_shouldMove = true;
	float m_attackTime = 2.f;
	float m_time = 0.f;
	Timer* m_timer;
	Map* m_map = nullptr;

	Hero( Game* game , Vec2 position, Faction faction=FACTION_GOOD );
	Hero* heroToAttack;


	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	void DebudRender();
	void TakeDamage( float damageAmt );
	virtual void Die() override;

	void PopulateInitalValuesBasedOnUpgradeStatus();

	void DebugRender();
	void ResetVertices();

};