#include "Game/Turret.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

Turret::Turret( Game* game , Vec2 position , Texture* sprite , Vec2 forwardVec )
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.179f , -0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.179f , -0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.179f , 0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.179f , -0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.179f , 0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.179f , 0.152f , 0.f ) , Rgba8( 255 , 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_game = game;
	m_position = position;
	m_sprite = sprite;
	m_forwardVec = forwardVec;
}

Turret::~Turret()
{

}

void Turret::Update( float deltaseconds )
{
	Vec2 final_position = m_forwardVec.GetRotatedDegrees( m_halfAngle );
	Vec2 initial_position = m_forwardVec.GetRotatedDegrees( -m_halfAngle );

	if ( !hasReachedInitial && !hasReachedFinal )
	{
		goalDegrees = ( initial_position ).GetAngleDegrees();
	}


	if ( abs( GetShortestAngularDisplacement( m_orientationDegrees , goalDegrees ) ) <= 1.f && !hasReachedFinal )
	{
		goalDegrees = ( final_position ).GetAngleDegrees();
		hasReachedFinal = true;
		hasReachedInitial = false;
	}

	if ( abs( GetShortestAngularDisplacement( m_orientationDegrees , goalDegrees ) ) <= 1.f && !hasReachedInitial )
	{
		goalDegrees = ( initial_position ).GetAngleDegrees();
		hasReachedInitial = true;
		hasReachedFinal = false;
	}

	m_orientationDegrees = GetTurnedToward( m_orientationDegrees , goalDegrees , 90.f * deltaseconds );

}

void Turret::Render()
{
	g_theRenderer->BindTexture( m_sprite );

	Vertex_PCU vertCopy[ 6 ];
	memcpy( vertCopy , m_vertices , sizeof( Vertex_PCU ) * 6 );

	TransformVertexArray( 6 , vertCopy , 3.2f , m_orientationDegrees , m_position );
	g_theRenderer->DrawVertexArray( 6 , vertCopy );

	g_theRenderer->BindTexture( nullptr );
}

void Turret::Die()
{

}

void Turret::DebugRender()
{

}
