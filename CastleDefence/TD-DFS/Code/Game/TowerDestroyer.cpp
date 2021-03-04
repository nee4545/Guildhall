#include "Game/TowerDestroyer.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Tower.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

TowerDestroyer::TowerDestroyer( Game* game , Vec2 position , Faction faction /*= FACTION_GOOD */,Upgrade upgrade, Lanes lane ):Entity(game,position)
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
	m_lane = lane;
	m_upgrade = upgrade;

	m_attackTimer = new Timer();
	m_attackClock = new Clock();

	m_attackTimer->SetSeconds( m_attackClock , 1.3f );
	PopulateStatsBasedOnUpgrade();
}

void TowerDestroyer::Update( float deltaSeconds )
{
	m_time += deltaSeconds;

	m_timeTolive -= deltaSeconds;

	if ( m_timeTolive <= 0.f )
	{
		Die();
	}

	Entity* towerToAttack = nullptr;

	towerToAttack = GetTowerToAttack();

	if ( towerToAttack != nullptr )
	{
		m_isWalking = false;
		m_isAttacking = true;
	}
	else
	{
		m_isWalking = true;
		m_isAttacking = false;
	}

	if ( m_isWalking )
	{
		if ( m_faction == FACTION_GOOD )
		{
			m_position.x -= m_speed * deltaSeconds;
		}
		else
			m_position.x += m_speed * deltaSeconds;
	}

	if ( m_isAttacking )
	{
		if ( m_attackTimer->HasElapsed() )
		{
			towerToAttack = GetTowerToAttack();
			if ( towerToAttack != nullptr )
			{
				towerToAttack->TakeDamage( m_damage );
			}
			m_attackTimer->Reset();
		}
	}

}

void TowerDestroyer::Render()
{
	Texture* tex = GetTextureBasedOnStatus();
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 28.f , 0.f , m_position );
	g_theRenderer->BindTexture( tex );
	if ( m_faction == FACTION_BAD )
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

void TowerDestroyer::Die()
{
	m_isGarbage = true;
}

void TowerDestroyer::PopulateStatsBasedOnUpgrade()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_timeTolive = 10.f;
		m_damage = 1.f;
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_timeTolive = 15.f;
		m_damage = 1.5f;
	}
}

void TowerDestroyer::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

Texture* TowerDestroyer::GetTextureBasedOnStatus()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		if ( m_isWalking )
		{
			return m_game->m_TowerDestroyer1WalkTex->GetSpriteTextureAtTime( m_time );
		}

		if ( m_isAttacking )
		{
			return m_game->m_TowerDestroyer1AttackTex->GetSpriteTextureAtTime( m_time );
		}
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		if ( m_isWalking )
		{
			return m_game->m_TowerDestroyer2WalkTex->GetSpriteTextureAtTime( m_time );
		}

		if ( m_isAttacking )
		{
			return m_game->m_TowerDestroyer2AttackTex->GetSpriteTextureAtTime( m_time );
		}
	}

	return nullptr;
}

Entity* TowerDestroyer::GetTowerToAttack()
{
	if ( m_lane == LANE1_1 && m_faction == FACTION_GOOD )
	{
		if ( m_game->m_enemyTower1 != nullptr )
		{
			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyTower1->m_position , 7.f ) )
			{
				return m_game->m_enemyTower1;
			}
		}
	}

	if ( m_lane == LANE1_2 && m_faction == FACTION_GOOD )
	{
		if ( m_game->m_enemyTower2 != nullptr )
		{
			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyTower2->m_position , 7.f ) )
			{
				return m_game->m_enemyTower2;
			}
		}
	}
	
	if ( m_lane == LANE1_1 && m_faction == FACTION_BAD )
	{
		if ( m_game->m_playerTower1 != nullptr )
		{
			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerTower1->m_position , 7.f ) )
			{
				return m_game->m_playerTower1;
			}
		}
	}

	if ( m_lane == LANE1_2 && m_faction == FACTION_BAD )
	{
		if ( m_game->m_playerTower2 != nullptr )
		{
			if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerTower2->m_position , 7.f ) )
			{
				return m_game->m_playerTower2;
			}
		}
	}


	return nullptr;
}
