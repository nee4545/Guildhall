#pragma once
#include "Game/Entity.hpp"


class Debris : public Entity
{
	Vertex_PCU vertices[3];
	Vertex_PCU vertices_original[3];
	float m_timeToLive=2.f;
	float debris_speed=10.f;
	
public:
	Rgba8 m_DebrisColor;


	Debris( Game* thegame, Vec2 position , Rgba8 color ):Entity( thegame, position )
	{
		m_DebrisColor=color;

		vertices[0]=Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );
		vertices[1]= Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );
		vertices[2]= Vertex_PCU( Vec3( -1.f, 0.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );

		vertices_original[0]=Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );
		vertices_original[1]= Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );
		vertices_original[2]= Vertex_PCU( Vec3( -1.f, 0.f, 0.f ), m_DebrisColor, Vec2( 0.f, 0.f ) );
	}

	void SpawnDebris(Vec2 pos);

	virtual void Die() override;

	virtual void Render() override;

	virtual void Update(float deltaseconds) override;

	void MoveDebris(float deltaseconds);

	void TransformVertexArray( int num_vertices, Vertex_PCU* Vert, float scale, float orientationDegrees, Vec2 translation );


};