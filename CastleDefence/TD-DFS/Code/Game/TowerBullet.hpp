#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class TowerBullet :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;

	bool moveForward = true;
	float m_timeTolive = 10.f;
	float m_speed = 0.f;
	float m_damage = 0.f;

	TowerBullet( Game* game , Vec2 position , Faction faction = FACTION_GOOD,Upgrade upg = UPGRADE_NONE, bool shouldMoveForward = false, float orientationDegrees=0.f );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void DebugRender();
	void ResetVertices();

	void PopulateStats();
	void DetectCollissions();

	Texture* GetTexture();
};