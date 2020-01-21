#pragma once
#include "Game/Entity.hpp"

class SnakePart
{
public:
	Vertex_PCU vertices[3];
	Vertex_PCU vertices_original[3];

	Vec2 part_position;
	float part_radius=1.2f;


	SnakePart();

	void TransformVertexArray(int numVertices,Vertex_PCU* vert, float scale, float rotation, Vec2& translation);

	void Render();

	void SetPosition();
	
	
};