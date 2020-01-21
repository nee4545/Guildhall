#pragma once
#include "Game/Entity.hpp"

class AirStrike : public Entity
{
	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];

public:
	AirStrike( Game* game, Vec2 position );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	void ResetVertices();
	void ResetUvs();

	Vec2 uvMins;
	Vec2 uvMaxs;

	float timeElapsedInSeconds =0.f;
	float timeToLive;
	
};