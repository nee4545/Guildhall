#include "Game/AirStrikePlane.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Engine/Audio/AudioSystem.hpp"

AirStrikePlane::AirStrikePlane( Game* game, Vec2 position ):Entity(game,position)
{
	m_vertices[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_vertices[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_vertices[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_vertices[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_vertices_original[0]=Vertex_PCU( Vec3( -0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 0.f ) );
	m_vertices_original[1]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices_original[2]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );
			  
	m_vertices_original[3]=Vertex_PCU( Vec3( 0.4f, -0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 0.f ) );
	m_vertices_original[4]=Vertex_PCU( Vec3( 0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 1.f, 1.f ) );
	m_vertices_original[5]=Vertex_PCU( Vec3( -0.4f, 0.4f, 0.f ), Rgba8( 255, 255, 255 ), Vec2( 0.f, 1.f ) );

	m_isPushedByEntities =true;
	m_isPushedByWalls = true;
	m_doesPushEntities = true;
	m_isHitByBullets = false;

	m_physicsRadius=0.3f;
	m_cosmeticRadius=0.4f;

	g_theAudio->PlaySound(PLANE_SOUND);
}

void AirStrikePlane::Update( float deltaSeconds )
{
	timeElapsedInSeconds+=deltaSeconds;

	if( m_position.x<0.f )
	{
		Die();
	}

	m_position.x-=3.5f*deltaSeconds;

	if( m_position.x <= m_airStrikePosition.x && !finishedStrike )
	{
		CreateAirStrike();
	}
}

void AirStrikePlane::Render()
{
	g_theRenderer->TransformVertexArray( 6, m_vertices, 2.f, m_orientationDegrees, m_position );
	Texture* tankBase=g_theRenderer->GetOrCreateTextureFromFile( "Data/Images/AirStrikePlane.png" );
	g_theRenderer->BindTexture( tankBase );
	g_theRenderer->DrawVertexArray( 6, m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();
}

void AirStrikePlane::Die()
{
	m_isGarbage = true;
}

void AirStrikePlane::ResetVertices()
{
	for( int index =0; index<6; index++ )
	{
		m_vertices[index] = m_vertices_original[index];
	}
}

void AirStrikePlane::CreateAirStrike()
{
	m_game->world->current_map->CreateAirStrike(m_airStrikePosition);
	finishedStrike = true;
}
