#pragma once
#include "Game/Entity.hpp"

class SpriteAnimDefTex;
class Texture;

class Dino :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	Faction m_faction = FACTION_GOOD;

	Dino( Game* game , Vec2 position , Faction faction = FACTION_GOOD );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	float m_speed = 1.f;
	float m_time = 0.f;

	void DebugRender();
	void ResetVertices();

	SpriteAnimDefTex* m_anims=nullptr;
	std::vector<Texture*> m_dinos;

};