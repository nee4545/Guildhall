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
	

	if( entityType == ENTITY_TYPE_GOOD_BULLET )
	{
		g_theRenderer->TransformVertexArray( 6, m_vertices, 1.f, m_orientationDegrees, m_position );
		Texture* tex=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/FriendlyBullet.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6, m_vertices );
		g_theRenderer->BindTexture( nullptr );
		ResetVertices();
	}

	if( entityType == ENTITY_TYPE_EVIL_BULLET )
	{
		g_theRenderer->TransformVertexArray( 6, m_vertices, 1.f, m_orientationDegrees, m_position );
		Texture* tex=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/EnemyBullet.png" );
		g_theRenderer->BindTexture( tex );
		g_theRenderer->DrawVertexArray( 6, m_vertices );
		g_theRenderer->BindTexture( nullptr );
		ResetVertices();
	}
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

void Bullet::ChangeDirection()
{
   m_orientationDegrees=180.f-m_orientationDegrees;
}

void Bullet::ChangeColorBasedOnType()
{
	if( entityType == ENTITY_TYPE_GOOD_BULLET )
	{
		for( int index=0; index< 6; index++ )
		{
			m_vertices[index].m_color=Rgba8(0,0,255,255);
			m_vertices_original[index].m_color=Rgba8(0,0,255,255);
		}
	}

	if( entityType == ENTITY_TYPE_EVIL_BULLET )
	{
		for( int index=0; index< 6; index++ )
		{
			m_vertices[index].m_color=Rgba8( 255, 0, 0, 255 );
			m_vertices_original[index].m_color=Rgba8( 255, 0, 0, 255 );
		}
	}
}

