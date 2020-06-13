
#include "Game/RangeHero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"


RangeHero::RangeHero( Game* game , Vec2 position , Faction faction/*=FACTION_GOOD */ ) :Entity( game , position )
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

	m_faction = faction;
	m_physicsRadius = 3.5f;
	PopulateHeroStatesBasedOnUpgrade();

}

void RangeHero::Update( float deltaSeconds )
{

	m_time += deltaSeconds;

	/*if ( isRunning )
	{
		if ( m_faction == FACTION_GOOD )
		{
			m_position.x += m_speed * deltaSeconds;
		}
		else
		{
			m_position.x -= m_speed * deltaSeconds;
		}
	}*/

	
}

void RangeHero::Render()
{
	AABB2 healthBar = AABB2( m_position.x - 2.5f , m_position.y + 3.f , m_position.x + 2.5f , m_position.y + 6.f );
	AABB2 actualHealth = healthBar;
	actualHealth.CarveBoxOffLeft( ( m_health / 5.f ) );

	g_theRenderer->DrawAABB2D( healthBar , Rgba8( 100 , 100 , 100 , 100 ) );
	g_theRenderer->DrawAABB2D( actualHealth , Rgba8( 255 , 0 , 0 , 100 ) );

	Texture* tex = GetTextureBasedOnCurrentState();
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 15.f , 0.f , m_position );
	g_theRenderer->BindTexture( tex );
	if ( m_faction == FACTION_GOOD )
	{
		g_theRenderer->DrawVertexArray( 6 , m_vertices );
	}
	else
	{
		m_vertices[ 0 ].m_uvTexCoords = Vec2( 1.f , 0.f );
		m_vertices[ 1 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 2 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		m_vertices[ 3 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 4 ].m_uvTexCoords = Vec2( 0.f , 1.f );
		m_vertices[ 5 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		g_theRenderer->DrawVertexArray( 6 , m_vertices );
	}
	g_theRenderer->BindTexture( nullptr );
	//DebugRender();
	ResetVertices();



}

void RangeHero::Die()
{
	m_isGarbage = true;
}

void RangeHero::TakeDamage( float damageAmt )
{
	m_health -= damageAmt;

	if ( m_health <= 0 )
	{
		Die();
	}
}

void RangeHero::DebugRender()
{
	g_theRenderer->DrawRing( m_position , m_physicsRadius , Rgba8( 100 , 100 , 100 , 255 ) , 0.1f );
}

void RangeHero::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

Texture* RangeHero::GetTextureBasedOnCurrentState()
{
	Texture* tex = nullptr;

	switch ( m_upgrade )
	{
	case UPGRADE_NONE:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade0RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgade0RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade0RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade0RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_SMALL:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade1RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgade1RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade1RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade1RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}

		break;
	case UPGRADE_MEDIUM:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade2RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade2RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade2RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade2RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_LARGE:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade3RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade3RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade3RangeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade3RangeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	default:
		break;
	}

	return tex;

}

void RangeHero::PopulateHeroStatesBasedOnUpgrade()
{
	m_health = 5.f;
	m_damage = 1.f;
	m_speed = 5.f;
	m_attackSpeed = 0.95f;
	m_attackTimer = new Timer();
	Clock* c = new Clock();
	m_attackTimer->SetSeconds( c , m_attackSpeed );
}

void RangeHero::Attack()
{

}

void RangeHero::DetectHeroToAttack()
{

}



