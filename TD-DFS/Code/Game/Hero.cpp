#include "Game/Hero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"


Hero::Hero( Game* game , Vec2 position , Faction faction/*=FACTION_GOOD */ ) :Entity( game , position )
{

	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.4f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

}

void Hero::Update( float deltaSeconds )
{
	m_time += deltaSeconds;
}

void Hero::Render()
{
	Texture* tex = GetTextureBasedOnCurrentState();
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 15.f , 0.f , m_position );
	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();

}

void Hero::Die()
{
	m_isGarbage = true;
}

void Hero::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

Texture* Hero::GetTextureBasedOnCurrentState()
{
	Texture* tex = nullptr;

	switch ( m_upgrade )
	{
	case UPGRADE_NONE:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade0MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade0MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_SMALL:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade1MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade1MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_MEDIUM:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade2MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade2MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_LARGE:
		break;
	default:
		break;
	}

	return tex;

}
