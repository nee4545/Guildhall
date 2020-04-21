#include "Game/Hero.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"

Hero::Hero( Game* game , Vec2 position , Faction faction/*=FACTION_GOOD */ ):Entity(game,position)
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
	PopulateInitalValuesBasedOnUpgradeStatus();

}

void Hero::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	if ( m_shouldMove )
	{
		if ( m_faction == FACTION_GOOD )
		{
			m_position.x += m_speed*deltaSeconds;
		}
		else
		{
			m_position.x -= m_speed*deltaSeconds;
		}
	}

	if ( heroToAttack == nullptr )
	{
		m_timer->Reset();
		m_shouldMove = true;

	}
	else
	{
		m_shouldMove = false;
		if ( m_timer->HasElapsed() )
		{
			heroToAttack->TakeDamage( 1.f );
			m_timer->Reset();
		}
		if ( heroToAttack->m_health <= 0.f )
		{
			heroToAttack = nullptr;
		}
	}
}

void Hero::Render()
{
	AABB2 healthBar = AABB2( m_position.x - 0.5f , m_position.y + 0.8f , m_position.x + 0.5f , m_position.y + 1.1f );
	AABB2 actualHealth = healthBar;
	actualHealth.CarveBoxOffLeft( (m_health / HERO_MAX_HEALTH)  );
	Texture* tex=nullptr;

	TransformVertexArray( 6 , m_vertices , 3.5f , 0.f , m_position );
	if ( m_faction == FACTION_GOOD )
	{
		if ( m_shouldMove )
		{
			tex = m_map->playerRunning->GetSpriteTextureAtTime( m_time );
		}
		else
		{
			tex = m_map->playerAttacking->GetSpriteTextureAtTime( m_time );
		}
	}
	else
	{
		m_vertices[ 0 ].m_uvTexCoords = Vec2( 1.f , 0.f );
		m_vertices[ 1 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 2 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		m_vertices[ 3 ].m_uvTexCoords = Vec2( 0.f , 0.f );
		m_vertices[ 4 ].m_uvTexCoords = Vec2( 0.f , 1.f );
		m_vertices[ 5 ].m_uvTexCoords = Vec2( 1.f , 1.f );
		if ( m_shouldMove )
		{
			tex = m_map->enemyRunning->GetSpriteTextureAtTime( m_time );
		}
		else
		{
			tex = m_map->enemyAttacking->GetSpriteTextureAtTime( m_time );
		}
	}

	g_theRenderer->BindTexture( tex );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	g_theRenderer->BindTexture( nullptr );
	ResetVertices();

	g_theRenderer->DrawAABB2D( healthBar , Rgba8( 100 , 100 , 100 , 100 ));
	g_theRenderer->DrawAABB2D( actualHealth , Rgba8( 255 , 0 , 0 , 100 ));

	//DebudRender();
}

void Hero::DebudRender()
{
	g_theRenderer->DrawRing( m_position , 0.5f , Rgba8( 255 , 255 , 255 , 255 ) , 0.1f );
}

void Hero::TakeDamage( float damageAmt )
{
	m_health -= damageAmt;

	if ( m_health <= 0 )
	{
		Die();
		if ( m_faction == FACTION_GOOD )
		{
			m_map->IncreasePlayerCoins( m_map->m_manager.heroHitPoints );
		}
		else
		{
			m_map->IncreaseEnemyCoins( m_map->m_manager.heroHitPoints );
		}
	}
}

void Hero::Die()
{
	m_isGarbage = true;
}

void Hero::PopulateInitalValuesBasedOnUpgradeStatus()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_health = 2.f;
		m_speed = 1.5f;
		m_attackTime = 0.7f;
	}

	Clock* c = new Clock();
	m_timer = new Timer();
	m_timer->SetSeconds( c , m_attackTime );
	
	//m_timer->
}

void Hero::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}
