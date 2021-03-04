#include "Game/Hero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/RangeHero.hpp"


Hero::Hero( Game* game , Vec2 position , Faction faction/*=FACTION_GOOD */,Lanes lane, Upgrade upgrade ) :Entity( game , position )
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
	PopulateHeroStatesBasedOnUpgrade();

	m_healbarBase = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_bg.png" );
	m_healthbar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_2.png" );

}

void Hero::Update( float deltaSeconds )
{
	if ( m_isGarbage )
	{
		return;
	}

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

	if ( m_faction == FACTION_GOOD )
	{
		if ( m_position.x >= 290.f )
		{
			m_game->m_enemyCastleHealth -= 10.f;
			Die();
		}
	}
	else
	{
		if ( m_position.x <= 20.f )
		{
			m_game->m_castleHealth -= 10.f;
			Die();
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
				if ( canCrit )
				{
					int randomNum =m_game->m_rng.RollRandomIntInRange( 0 , 1 );

					if ( randomNum == 0 )
					{
						float damage = m_damage * 3;
						m_heroToAttack->TakeDamage( damage );
					}
					else
						m_heroToAttack->TakeDamage( m_damage );
				}
				else
				{
					m_heroToAttack->TakeDamage( m_damage );
				}
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

	Texture* tex = GetTextureBasedOnCurrentState();
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 17.f , 0.f , m_position );
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
	AABB2 healthBar = AABB2( m_position.x - 3.f , m_position.y + 2.f , m_position.x + 3.f , m_position.y + 4.5f );
	AABB2 actualHealth = healthBar.GetPercentageOfAABB( 0.9f );
	actualHealth.CarveBoxOffLeft( ( m_health / m_totalHealth ) );

	g_theRenderer->BindTexture( m_healbarBase );
	g_theRenderer->DrawAABB2D( healthBar , Rgba8( 255 , 255 , 255 , 125 ) );
	g_theRenderer->BindTexture( m_healthbar );
	g_theRenderer->DrawAABB2D( actualHealth , Rgba8( 255 , 255 , 255 , 125 ) );
	g_theRenderer->BindTexture( nullptr );
	//DebugRender();
	ResetVertices();

	
}

void Hero::Die()
{
	switch ( m_upgrade )
	{
	case UPGRADE_NONE:
	{
		m_game->m_playerCoins += MELEE_UPGRADE0_DEATH_COINS;
		break;
	}
	case UPGRADE_SMALL:
	{
		m_game->m_playerCoins += MELEE_UPGRADE1_DEATH_COINS;
		break;
	}
	case UPGRADE_MEDIUM:
	{
		m_game->m_playerCoins += MELEE_UPGRADE2_DEATH_COINS;
		break;
	}
	case UPGRADE_LARGE:
	{
		m_game->m_playerCoins += MELEE_UPGRADE3_DEATH_COINS;
		break;
	}
	default:
		break;
	}

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
	g_theRenderer->DrawRing( m_position , m_physicsRadius , Rgba8( 255 , 255 , 255 , 255 ),0.1f );
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
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_health = 5.f;
		m_totalHealth = 5.f;
		m_damage = 1.f;
		m_speed = 5.f;
		m_attackSpeed = 0.95f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_health = 7.f;
		m_totalHealth = 7.f;
		m_damage = 1.f;
		m_speed = 7.5f;
		m_attackSpeed = 0.65f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_animationSpeed = 0.65f;
	}

	if ( m_upgrade == UPGRADE_MEDIUM )
	{
		m_health = 15.f;
		m_totalHealth = 15.f;
		m_damage = 2.5f;
		m_speed = 8.5f;
		m_attackSpeed = 0.65f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_animationSpeed = 0.65f;
	}

	if ( m_upgrade == UPGRADE_LARGE )
	{
		m_health = 15.f;
		m_totalHealth = 15.f;
		m_damage = 3.f;
		m_speed = 8.5f;
		m_attackSpeed = 0.65f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_animationSpeed = 0.65f;
		canCrit = true;
	}

}

void Hero::GetHeroToAttack()
{
	if ( m_faction == FACTION_GOOD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_1[ i ]->m_position , m_game->m_enemyMeleeHerosLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyMeleeHerosLane1_1[ i ];
					return;
				}
			}

			for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_1[ i ]->m_position , m_game->m_enemyRangeHerosLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyRangeHerosLane1_1[ i ];
					return;
				}
			}

		}

		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_2[ i ]->m_position , m_game->m_enemyMeleeHerosLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyMeleeHerosLane1_2[ i ];
					return;
				}
			}

			for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_2[ i ]->m_position , m_game->m_enemyRangeHerosLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyRangeHerosLane1_2[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_1[ i ]->m_position , m_game->m_enemyMeleeHerosLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyMeleeHerosLane2_1[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_1[ i ]->m_position , m_game->m_enemyRangeHerosLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyRangeHerosLane2_1[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE2_2 )
		{
			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane2_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_2[ i ]->m_position , m_game->m_enemyMeleeHerosLane2_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyMeleeHerosLane2_2[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane2_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_2[ i ]->m_position , m_game->m_enemyRangeHerosLane2_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_enemyRangeHerosLane2_2[ i ];
					return;
				}
			}
		}

	}


	if ( m_faction == FACTION_BAD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_1[ i ]->m_position , m_game->m_playerMeleeHerosLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerMeleeHerosLane1_1[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_playerRangeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_1[ i ]->m_position , m_game->m_playerRangeHerosLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerRangeHerosLane1_1[ i ];
					return;
				}
			}
		}


		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_2[ i ]->m_position , m_game->m_playerMeleeHerosLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerMeleeHerosLane1_2[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_playerRangeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_2[ i ]->m_position , m_game->m_playerRangeHerosLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerRangeHerosLane1_2[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_1[ i ]->m_position , m_game->m_playerMeleeHerosLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerMeleeHerosLane2_1[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_playerRangeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_1[ i ]->m_position , m_game->m_playerRangeHerosLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerRangeHerosLane2_1[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE2_2 )
		{
			for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane2_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_2[ i ]->m_position , m_game->m_playerMeleeHerosLane2_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerMeleeHerosLane2_2[ i ];
					return;
				}

			}

			for ( int i = 0; i < m_game->m_playerRangeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane2_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_2[ i ]->m_position , m_game->m_playerRangeHerosLane2_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_playerRangeHerosLane2_2[ i ];
					return;
				}
			}
		}

	}
}
