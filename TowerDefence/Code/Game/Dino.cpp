#include "Game/Dino.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"

Dino::Dino( Game* game , Vec2 position , Faction faction /*= FACTION_GOOD */ ):Entity(game,position)
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.6f , -0.5f , 0.f ) , Rgba8( 200, 100 , 0 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.6f , -0.5f , 0.f ) , Rgba8(  200, 100 , 0 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.6f , 0.5f , 0.f ) , Rgba8(  200, 100 , 0 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.6f , -0.5f , 0.f ) , Rgba8(200 ,100 ,0 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.6f , 0.5f , 0.f ) , Rgba8( 200, 100, 0) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.6f , 0.5f , 0.f ) , Rgba8(200 ,100 ,0 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.6f , -0.5f , 0.f ) , Rgba8(200 ,100 ,0 ) , Vec2( 0.f , 0.f ) );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.6f , -0.5f , 0.f ) , Rgba8( 200, 100, 0) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.6f , 0.5f , 0.f ) , Rgba8( 200, 100, 0) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.6f , -0.5f , 0.f ) , Rgba8(200 ,100 ,0 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.6f , 0.5f , 0.f ) , Rgba8( 200, 100, 0) , Vec2( 1.f , 1.f ) );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.6f , 0.5f , 0.f ) , Rgba8(200 ,100 ,0 ) , Vec2( 0.f , 1.f ) );

	m_faction = faction;

	Texture* t1 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r1.png" );
	Texture* t2 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r2.png" );
	Texture* t3 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r3.png" );
	Texture* t4 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r4.png" );
	Texture* t5 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r5.png" );
	Texture* t6 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r6.png" );
	Texture* t7 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r7.png" );
	Texture* t8 = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Animation/Dino/r8.png" );

	m_dinos.push_back( t1 );
	m_dinos.push_back( t2 );
	m_dinos.push_back( t3 );
	m_dinos.push_back( t4 );
	m_dinos.push_back( t5 );
	m_dinos.push_back( t6 );
	m_dinos.push_back( t7 );
	m_dinos.push_back( t8 );

	m_anims = new SpriteAnimDefTex( 0 , m_dinos.size() - 1 , 1.f );
	m_anims->m_animations = m_dinos;
}

void Dino::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	if ( m_faction == FACTION_GOOD )
	{
		m_position.x += ( m_speed + DINO_SPEED ) * deltaSeconds;
	}
	else
	{
		m_position.x -= ( m_speed + DINO_SPEED ) * deltaSeconds;
	}
}

void Dino::Render()
{
	TransformVertexArray( 6 , m_vertices , 3.5f , 0.f , m_position );
	Texture* tex = nullptr;
	if ( m_faction == FACTION_GOOD )
	{
		m_vertices[ 0 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		m_vertices[ 1 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		m_vertices[ 2 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		m_vertices[ 3 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		m_vertices[ 4 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		m_vertices[ 5 ].m_color = Rgba8( 255 , 255 , 255 , 255 );
		tex = m_anims->GetSpriteTextureAtTime( m_time );
	}
	else
	{
		m_vertices[ 0 ].m_uvTexCoords = Vec2( 1.f , 0.f );
		m_vertices[ 1 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 2 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		m_vertices[ 3 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 4 ].m_uvTexCoords = Vec2( 0.f , 1.f );
		m_vertices[ 5 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		tex = m_anims->GetSpriteTextureAtTime( m_time );
	}

	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();
}

void Dino::Die()
{
	m_isGarbage = true;
}

void Dino::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}
