#pragma once
#include "Game/Entity.hpp"

class Texture;

class Hero :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];
	float m_time = 0.f;

	bool isAttacking = false;
	bool isRunning = true;

	Faction m_faction = FACTION_GOOD;
	Upgrade m_upgrade = UPGRADE_NONE;

	Hero( Game* game , Vec2 position , Faction faction = FACTION_GOOD );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void DebugRender();
	void ResetVertices();
	Texture* GetTextureBasedOnCurrentState();

};