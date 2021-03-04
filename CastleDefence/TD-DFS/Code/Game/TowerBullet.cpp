#include "Game/TowerBullet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Game.hpp"
#include "Game/Hero.hpp"
#include "Game/RangeHero.hpp"
#include "Game/UncontrolledHero.hpp"

TowerBullet::TowerBullet( Game* game , Vec2 position , Faction faction /*= FACTION_GOOD */, Upgrade upg ,bool shouldMoveForward , float orientationDegrees ):Entity(game,position)
{
	m_vertices[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 0 ] = Vertex_PCU( Vec3( -0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 0.f ) );
	m_vertices_original[ 1 ] = Vertex_PCU( Vec3( 0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 2 ] = Vertex_PCU( Vec3( -0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	m_vertices_original[ 3 ] = Vertex_PCU( Vec3( 0.4f , -0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 0.f ) );
	m_vertices_original[ 4 ] = Vertex_PCU( Vec3( 0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 1.f , 1.f ) );
	m_vertices_original[ 5 ] = Vertex_PCU( Vec3( -0.4f , 0.2f , 0.f ) , Rgba8( 255 , 255 , 255 ) , Vec2( 0.f , 1.f ) );

	moveForward = shouldMoveForward;
	m_orientationDegrees = orientationDegrees;
	m_upgrade = upg;
	m_physicsRadius = 3.f;
	m_isGarbage = false;
	m_faction = faction;

	PopulateStats();
}

void TowerBullet::Update( float deltaSeconds )
{
	m_timeTolive -= deltaSeconds;

	if ( m_timeTolive <= 0.f )
	{
		Die();
	}

	m_position.x += CosDegrees( m_orientationDegrees ) * deltaSeconds * m_speed;
	m_position.y += SinDegrees( m_orientationDegrees ) * deltaSeconds * m_speed;
	
	DetectCollissions();
}

void TowerBullet::Render()
{
	g_theRenderer->BindTexture( GetTexture() );

	g_theRenderer->TransformVertexArray( 6 , m_vertices , 6.f , m_orientationDegrees , m_position );

	g_theRenderer->DrawVertexArray( 6 , m_vertices );

	g_theRenderer->BindTexture( nullptr );

	//DebugRender();
	ResetVertices();
}

void TowerBullet::Die()
{
	m_isGarbage = true;
}

void TowerBullet::DebugRender()
{
	g_theRenderer->DrawRing( m_position , 3.f , Rgba8( 255 , 255 , 255 , 255 ) , 0.4f );
}

void TowerBullet::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

void TowerBullet::PopulateStats()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_damage = 0.5f;
		m_speed = 30.f;
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_damage = 1.f;
		m_speed = 35.f;
	}

	if ( m_upgrade == UPGRADE_MEDIUM )
	{
		m_damage = 2.f;
		m_speed = 40.f;
	}

	if ( m_upgrade == UPGRADE_LARGE )
	{
		m_damage = 4.f;
		m_speed = 45.f;
	}
}

void TowerBullet::DetectCollissions()
{
	if ( m_isGarbage )
	{
		return;
	}

	if ( m_faction == FACTION_BAD )
	{
		for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_1.size(); i++ )
		{
			if ( m_game->m_playerMeleeHerosLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerMeleeHerosLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_2.size(); i++ )
		{
			if ( m_game->m_playerMeleeHerosLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerMeleeHerosLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_1.size(); i++ )
		{
			if ( m_game->m_playerMeleeHerosLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerMeleeHerosLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_2.size(); i++ )
		{
			if ( m_game->m_playerMeleeHerosLane2_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerMeleeHerosLane2_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerRangeHerosLane1_1.size(); i++ )
		{
			if ( m_game->m_playerRangeHerosLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerRangeHerosLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerRangeHerosLane1_2.size(); i++ )
		{
			if ( m_game->m_playerRangeHerosLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerRangeHerosLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerRangeHerosLane2_1.size(); i++ )
		{
			if ( m_game->m_playerRangeHerosLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerRangeHerosLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_playerRangeHerosLane2_2.size(); i++ )
		{
			if ( m_game->m_playerRangeHerosLane2_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_playerRangeHerosLane2_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UplayerLane1_1.size(); i++ )
		{
			if ( m_game->m_UplayerLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UplayerLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UplayerLane1_2.size(); i++ )
		{
			if ( m_game->m_UplayerLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UplayerLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UplayerLane2_1.size(); i++ )
		{
			if ( m_game->m_UplayerLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UplayerLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}
	}

	if ( m_faction == FACTION_GOOD )
	{
		for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_1.size(); i++ )
		{
			if ( m_game->m_enemyMeleeHerosLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyMeleeHerosLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_2.size(); i++ )
		{
			if ( m_game->m_enemyMeleeHerosLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyMeleeHerosLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_1.size(); i++ )
		{
			if ( m_game->m_enemyMeleeHerosLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyMeleeHerosLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_2.size(); i++ )
		{
			if ( m_game->m_enemyMeleeHerosLane2_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyMeleeHerosLane2_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_1.size(); i++ )
		{
			if ( m_game->m_enemyRangeHerosLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyRangeHerosLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_2.size(); i++ )
		{
			if ( m_game->m_enemyRangeHerosLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyRangeHerosLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_1.size(); i++ )
		{
			if ( m_game->m_enemyRangeHerosLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyRangeHerosLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_2.size(); i++ )
		{
			if ( m_game->m_enemyRangeHerosLane2_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_enemyRangeHerosLane2_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UenemyLane1_1.size(); i++ )
		{
			if ( m_game->m_UenemyLane1_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane1_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UenemyLane1_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UenemyLane1_2.size(); i++ )
		{
			if ( m_game->m_UenemyLane1_2[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane1_2[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UenemyLane1_2[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}

		for ( int i = 0; i < m_game->m_UenemyLane2_1.size(); i++ )
		{
			if ( m_game->m_UenemyLane2_1[ i ] != nullptr )
			{
				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane2_1[ i ]->m_position , m_physicsRadius ) )
				{
					m_game->m_UenemyLane2_1[ i ]->TakeDamage( m_damage );
					Die();
					return;
				}
			}
		}
		
	}

}

Texture* TowerBullet::GetTexture()
{
	switch ( m_upgrade )
	{
	case UPGRADE_NONE:
	{
		return m_game->m_bullet0tex;
		break;
	}
	case UPGRADE_SMALL:
	{
		return m_game->m_bullet1tex;
		break;
	}
	case UPGRADE_MEDIUM:
	{
		return m_game->m_bullet2tex;
		break;
	}
	case UPGRADE_LARGE:
	{
		return m_game->m_bullet3tex;
		break;
	}
	default:
	{
		return nullptr;
		break;
	}
	}
}
