#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

Player::Player( Game* game, Vec2 position ):Entity(game,position)
{
	m_vertices[0]=Vertex_PCU(Vec3(-0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,0.f));
	m_vertices[1]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[2]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_vertices[3]=Vertex_PCU(Vec3(0.4f,-0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,0.f));
	m_vertices[4]=Vertex_PCU(Vec3(0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(1.f,1.f));
	m_vertices[5]=Vertex_PCU(Vec3(-0.4f,0.4f,0.f),Rgba8(255,255,255),Vec2(0.f,1.f));

	m_vertices_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_vertices_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
		  
	m_vertices_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_vertices_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_orientationDegrees=0.f;
	m_physicsRadius=0.2f;
	m_cosmeticRadius=0.4f;
}

void Player::Update( float deltaSeconds )
{
	if ( m_isGarbage )
	{
		return;
	}

	if ( g_theInput->IsKeyPressed( 'W' ) )
	{
		m_position.y += PLAYER_SPEED * deltaSeconds;
	}

	if ( g_theInput->IsKeyPressed( 'S' ) )
	{
		m_position.y -= PLAYER_SPEED * deltaSeconds;
	}

	if ( g_theInput->IsKeyPressed( 'A' ) )
	{
		m_position.x -= PLAYER_SPEED * deltaSeconds;
	}
	
	if ( g_theInput->IsKeyPressed( 'D' ) )
	{
		m_position.x += PLAYER_SPEED * deltaSeconds;
	}
}

void Player::Render() 
{
	if ( m_isGarbage )
	{
		return;
	}

	g_theRenderer->TransformVertexArray( 6 , m_vertices , 1.f , 0.f , m_position );
	Texture* tex = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/bbear.png" );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();
}

void Player::Die()
{
	m_isGarbage = true;
}

void Player::DebugRender()
{

}

void Player::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

