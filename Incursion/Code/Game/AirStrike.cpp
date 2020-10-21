#include"Game/AirStrike.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

AirStrike::AirStrike( Game* game, Vec2 position ):Entity(game,position)
{
	m_vertices[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMins );
	m_vertices[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMaxs );
	m_vertices[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMins );
	m_vertices_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_vertices_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMaxs.x, uvMins.y ) );
	m_vertices_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), uvMaxs );
	m_vertices_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( uvMins.x, uvMaxs.y ) );

	m_orientationDegrees = 0.f;
}

void AirStrike::Update( float deltaSeconds )
{
	timeElapsedInSeconds += deltaSeconds;

	if( timeElapsedInSeconds > 1.f )
	{
		Die();
	}
}

void AirStrike::Render()
{
	Texture* texture = g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/Explosion_5x5.png" );
	SpriteSheet* spriteSheet = new SpriteSheet( *texture, IntVec2( 5, 5 ) );
	SpriteAnimDefinition anim = SpriteAnimDefinition( *spriteSheet, 0, 24, 1.f );
	const SpriteDefinition& expl = anim.GetSpriteDefAtTime( timeElapsedInSeconds );

	expl.GetUVs( uvMins, uvMaxs );
	ResetUvs();

	g_theRenderer->TransformVertexArray( 6, m_vertices, 3.f, m_orientationDegrees, m_position );
	g_theRenderer->BindTexture( texture );
	g_theRenderer->SetBlendMode( BlendMode::ADDITIVE );
	g_theRenderer->DrawVertexArray( 6, m_vertices );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->SetBlendMode( BlendMode::ALPHA );
	
	ResetVertices();

	
}

void AirStrike::Die()
{
	m_isGarbage = true;
}

void AirStrike::ResetVertices()
{
	for( int index =0; index<6; index++ )
	{
		m_vertices[index] = m_vertices_original[index];
	}
}

void AirStrike::ResetUvs()
{
	m_vertices[0].m_uvTexCoords = uvMins;
	m_vertices[1].m_uvTexCoords = Vec2( uvMaxs.x, uvMins.y );
	m_vertices[2].m_uvTexCoords = Vec2( uvMins.x, uvMaxs.y );
	m_vertices[3].m_uvTexCoords = Vec2( uvMaxs.x, uvMins.y );
	m_vertices[4].m_uvTexCoords = uvMaxs;
	m_vertices[5].m_uvTexCoords = Vec2( uvMins.x, uvMaxs.y );
}
