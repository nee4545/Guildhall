#pragma once
#include "Game/Entity.hpp"

class Game;

class Collectable :public Entity
{
public:

	Collectable( Game* game , Vec2 position );

	Vertex_PCU m_vertices[ 6 ];
	Vertex_PCU m_vertices_original[ 6 ];


	virtual void Update( float deltaSeconds ) override;
	virtual void Render() override;
	virtual void Die() override;
	void ResetVertices();

};