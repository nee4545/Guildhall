#pragma once
#include "Game/Entity.hpp"


class Wall:public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Faction m_faction = FACTION_GOOD;

	Wall( Game* game , Vec2 position , Faction faction = FACTION_GOOD );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void DebugRender();
	void ResetVertices();
};