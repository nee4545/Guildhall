#pragma once
#include "Entity.hpp"
#include "Engine/Core/AABB2.hpp"

class Player:public Entity
{
public:

	Player(Game* game,Vec2 position);

	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];

	float m_turretAngle=0.f;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() override;
	virtual void Die() override;

	void DebugRender();
	void ResetVertices();
};
