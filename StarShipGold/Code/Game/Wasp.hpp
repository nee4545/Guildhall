#pragma once
#include "Game/Entity.hpp"

class Wasp : public Entity
{
	Vertex_PCU vertices[12];
	Vertex_PCU vertices_original[12];

	float wasp_accleration=20.f;
	float max_velocity=30.f;

public:
	Wasp( Game* thegame, Vec2 position ):Entity( thegame, position )
	{
		Rgba8 color=Rgba8( 0, 0, 150, 255 );

		vertices[0]=Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[1]= Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[2]= Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), color, Vec2( 0.f, 0.f ) );

		vertices[3]=Vertex_PCU( Vec3( 1.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[4]= Vertex_PCU( Vec3( 1.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[5]= Vertex_PCU( Vec3( 2.f, 0.f, 0.f ), color, Vec2( 0.f, 0.f ) );

		vertices[6]=Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[7]= Vertex_PCU( Vec3( -1.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[8]= Vertex_PCU( Vec3( -1.f, -2.f, 0.f ), color, Vec2( 0.f, 0.f ) );

		vertices[9]=Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[10]= Vertex_PCU( Vec3( -1.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices[11]= Vertex_PCU( Vec3( -1.f, 2.f, 0.f ), color, Vec2( 0.f, 0.f ) );

		vertices_original[0]=Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[1]= Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[2]= Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), color, Vec2( 0.f, 0.f ) );
				
		vertices_original[3]=Vertex_PCU( Vec3( 1.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[4]= Vertex_PCU( Vec3( 1.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[5]= Vertex_PCU( Vec3( 2.f, 0.f, 0.f ), color, Vec2( 0.f, 0.f ) );
				
		vertices_original[6]=Vertex_PCU( Vec3( 0.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[7]= Vertex_PCU( Vec3( -1.f, -1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[8]= Vertex_PCU( Vec3( -1.f, -2.f, 0.f ), color, Vec2( 0.f, 0.f ) );
				
		vertices_original[9]=Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[10]= Vertex_PCU( Vec3( -1.f, 1.f, 0.f ), color, Vec2( 0.f, 0.f ) );
		vertices_original[11]= Vertex_PCU( Vec3( -1.f, 2.f, 0.f ), color, Vec2( 0.f, 0.f ) );

		m_physicsRadius=1.5f;
		m_cosmeticRadius=2.f;
		m_health=3;
	}

	void FollowPlayerShip();

	void MoveWasp( float deltaseconds );

	virtual void Update( float deltaseconds ) override;

	virtual void Render() override;

	virtual void Die() override;

	void TransformVertexArray( int num_vertices, Vertex_PCU* Vert, float scale, float orientationDegrees, Vec2 translation );
};

