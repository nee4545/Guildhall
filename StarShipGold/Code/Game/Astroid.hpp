#pragma once
#include "Game/Entity.hpp"

class Astroid: public Entity
{
public:
	Vertex_PCU vertices[48];
	Vertex_PCU vertices_original[48];
	Astroid( Game* thegame, Vec2 position );

	void SetRandomAngularVelocity();
	void SetRandomLinearVelocity();
	void MoveAstroid(float deltaseconds);
	void BounceOffWall();

	void Spin(float deltaseconds);

	void TransformVertexArray(int num_ship_vertices, Vertex_PCU* astroidVert, float scale, float orientationDegrees, Vec2 translation);

	virtual void Update(float deltaseconds) override;
	virtual void Render()override;
	virtual void Die() override;
};


