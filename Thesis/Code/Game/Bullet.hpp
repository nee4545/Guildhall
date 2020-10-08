#pragma once
#include "Game/Entity.hpp"

class Texture;

enum Faction
{
	FACTION_GOOD,
	FACTION_BAD,
};

class Bullet :public Entity
{
public:
	Bullet(Texture* sprite,Vec2 forwardVec,float speed,Faction faction=FACTION_GOOD);
	~Bullet();

	Vertex_PCU m_vertices[ 6 ];

	virtual void Update( float deltaseconds );
	virtual void Render();
	virtual void DebugRender();
	virtual void Die();

	Faction m_faction = FACTION_GOOD;
	Texture* m_sprite = nullptr;
	Vec2 m_forwardVec = Vec2( 0.f , 1.f );
	float m_speed = 10.f;
	
};