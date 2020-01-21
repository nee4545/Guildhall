#pragma once
#include "Game/Entity.hpp"

class Explosion : public Entity
{
	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];

	

public:
	Explosion( Game* game, Vec2 position );
	float timeElapsedInSeconds =0.f;
	float timeToLive;
	float explosionSize;

	virtual void Update( float deltaSeconds ) override;
	void Render() override;
	virtual void Die() override;
	Vec2 uvMins;
	Vec2 uvMaxs;
	void ResetVertices();
	void ResetUvs();

	
};