#pragma once
#include "Game/Entity.hpp"

class AirStrikePlane : public Entity
{
	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];

public:
	AirStrikePlane( Game* game, Vec2 position );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void ResetVertices();
	void CreateAirStrike();

	float timeElapsedInSeconds =0.f;
	Vec2 m_airStrikePosition;
	bool finishedStrike =false;
};