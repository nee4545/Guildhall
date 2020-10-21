#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public:
	Bullet( Game* game, Vec2 position );

	Vertex_PCU m_vertices[6];
	Vertex_PCU m_vertices_original[6];

	float bulletSpeed=0.f;

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void MoveBullet(float deltaSeconds);
	void ResetVertices();
	void ChangeDirection();
};