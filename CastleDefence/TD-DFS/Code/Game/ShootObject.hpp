#pragma once

#include "Game/Entity.hpp"

class Texture;

enum ShootPower
{
	POWER_0,
	POWER_1,
	POWER_2
};

class ShootObject :public Entity
{
public:
	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];

	float m_speed = 25.f;
	float m_damage = 1.f;

	Faction m_faction = FACTION_GOOD;
	ShootPower m_power = POWER_0;
	Lanes m_lane = LANE1_1;
	Texture* m_texture = nullptr;

	ShootObject( Game* game , Vec2 position ,Texture* tex, Faction faction = FACTION_GOOD,Lanes lane=LANE1_1 );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void DebugRender();
	void ResetVertices();
	void DetectCollission();
	
};