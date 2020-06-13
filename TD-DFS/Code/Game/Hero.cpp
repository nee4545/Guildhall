#include "Game/Hero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"


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

	m_faction = faction;
	m_physicsRadius = 3.5f;
	PopulateHeroStatesBasedOnUpgrade();

}

void Hero::Update( float deltaSeconds )
{

	m_time += deltaSeconds;
	GetHeroToAttack();

	if ( isRunning )
	{
		if ( m_faction == FACTION_GOOD )
		{
			m_position.x += m_speed * deltaSeconds;
		}
		else
		{
			m_position.x -= m_speed * deltaSeconds;
		}
	}

	if ( m_heroToAttack == nullptr )
	{
		m_attackTimer->Reset();
		isRunning = true;
		isAttacking = false;
	}
	else
	{
		isRunning = false;
		isAttacking = true;
		if ( m_attackTimer->HasElapsed() )
		{
			if ( m_heroToAttack != nullptr )
			{
				m_heroToAttack->TakeDamage( 1.f );
			}
			m_attackTimer->Reset();
		}
		if ( m_heroToAttack->m_health <= 0.f )
		{
			m_heroToAttack = nullptr;
		}
	}
}

void Hero::Render()
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

void Hero::Die()
{
	m_isGarbage = true;
}

void Hero::TakeDamage( float damageAmt )
{
	m_health -= damageAmt;

	if ( m_health <= 0 )
	{
		Die();
	}
}

void Hero::DebugRender()
{
	g_theRenderer->DrawRing( m_position , m_physicsRadius , Rgba8( 100 , 100 , 100 , 255 ),0.1f );
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
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade0MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade0MeleeAttack->GetSpriteTextureAtTime( m_time );
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
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade1MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade1MeleeAttack->GetSpriteTextureAtTime( m_time );
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
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade2MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade2MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	case UPGRADE_LARGE:
		if ( m_faction == FACTION_GOOD )
		{
			if ( isRunning )
			{
				tex = m_game->m_PlayerUpgrade3MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_PlayerUpgrade3MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				tex = m_game->m_EnemyUpgrade3MeleeRun->GetSpriteTextureAtTime( m_time );
			}

			if ( isAttacking )
			{
				tex = m_game->m_EnemyUpgrade3MeleeAttack->GetSpriteTextureAtTime( m_time );
			}
		}
		break;
	default:
		break;
	}

	return tex;

}

void Hero::PopulateHeroStatesBasedOnUpgrade()
{
	m_health = 5.f;
	m_damage = 1.f;
	m_speed = 5.f;
	m_attackSpeed = 0.95f;
	m_attackTimer = new Timer( );
	Clock* c = new Clock();
	m_attackTimer->SetSeconds( c , m_attackSpeed );
}

void Hero::GetHeroToAttack()
{
	if ( m_faction == FACTION_GOOD )
	{
		for ( int i = 0; i < m_game->m_enemyHeros.size(); i++ )
		{
			if ( m_game->m_enemyHeros[ i ] == nullptr )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyHeros[ i ]->m_position , m_game->m_enemyHeros[ i ]->m_physicsRadius ) )
			{
				m_heroToAttack = m_game->m_enemyHeros[ i ];
				return;
			}
			
		}
	}


	if ( m_faction == FACTION_BAD )
	{
		for ( int i = 0; i < m_game->m_playerMeleeHeros.size(); i++ )
		{
			if ( m_game->m_playerMeleeHeros[ i ] == nullptr )
			{
				continue;
			}

			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHeros[ i ]->m_position , m_game->m_playerMeleeHeros[ i ]->m_physicsRadius ) )
			{
				m_heroToAttack = m_game->m_playerMeleeHeros[ i ];
				return;
			}

		}
	}
}
