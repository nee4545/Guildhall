#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

Bullet::Bullet( Game* game, Vec2 position ):Entity(game,position)
{
	Rgba8 color=Rgba8(255,255,255,255);
	

	m_vertices[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f )*0.1f,color , Vec2( 0.f, 0.f ) );
	m_vertices[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f )*0.1f, color, Vec2( 1.f, 0.f ) );
	m_vertices[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 0.f, 1.f ) );
	 
	m_vertices[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f )*0.1f, color, Vec2( 1.f, 0.f ) );
	m_vertices[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 1.f, 1.f ) );
	m_vertices[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 0.f, 1.f ) );

	m_vertices_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f )*0.1f, color, Vec2( 0.f, 0.f ) );
	m_vertices_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f )*0.1f, color, Vec2( 1.f, 0.f ) );
	m_vertices_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 0.f, 1.f ) );
		
	m_vertices_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f )*0.1f, color, Vec2( 1.f, 0.f ) );
	m_vertices_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 1.f, 1.f ) );
	m_vertices_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f )*0.1f, color, Vec2( 0.f, 1.f ) );


	bulletSpeed=BULLET_SPEED;
	
}			 

void Bullet::Update( float deltaSeconds )
{
	MoveBullet(deltaSeconds);
}

void Bullet::Render()
{
	

}

void Bullet::Die()
{
	m_isGarbage=true;
}

void Bullet::MoveBullet(float deltaSeconds)
{
	m_position.x+=bulletSpeed*CosDegrees(m_orientationDegrees)*deltaSeconds;
	m_position.y+=bulletSpeed*SinDegrees(m_orientationDegrees)*deltaSeconds;
}

void Bullet::ResetVertices()
{
	for( int index=0; index<6; index++ )
	{
		m_vertices[index]=m_vertices_original[index];
	}
}


