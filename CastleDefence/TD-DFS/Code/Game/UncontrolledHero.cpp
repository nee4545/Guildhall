#include "Game/UncontrolledHero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/RangeHero.hpp"


UncontrolledHero::UncontrolledHero( Game* game , Vec2 position , Faction faction/*=FACTION_GOOD */, Lanes lane, bool isUp ) :Entity( game , position )
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
	isSpecial = isUp;
	m_lane = lane;

	PopulateHeroStatesBasedOnUpgrade();

	m_healbarBase = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_bg.png" );
	m_healthbar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_2.png" );

}

void UncontrolledHero::Update( float deltaSeconds )
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
			m_game->m_enemyCastleHealth -= 0.3f;
			Die();
		}
	}
	else
	{
		if ( m_position.x <= 20.f )
		{
			m_game->m_castleHealth -= 0.2f;
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
			GetHeroToAttack();
			if ( m_heroToAttack != nullptr )
			{
				m_heroToAttack->TakeDamage( m_damage );
			}
			m_attackTimer->Reset();
		}
		if ( m_heroToAttack->m_health <= 0.f )
		{
			m_heroToAttack = nullptr;
		}
	}
}

void UncontrolledHero::Render()
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

void UncontrolledHero::Die()
{
	if ( isSpecial )
	{
		m_game->m_playerCoins += 2;
	}
	else
	{
		m_game->m_playerCoins += 3;
	}

	m_isGarbage = true;
}

void UncontrolledHero::TakeDamage( float damageAmt )
{
	m_health -= damageAmt;

	if ( m_health <= 0 )
	{
		Die();
	}
}

void UncontrolledHero::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

Texture* UncontrolledHero::GetTextureBasedOnCurrentState()
{
	if ( m_faction == FACTION_GOOD )
	{
		if ( !isSpecial )
		{
			if ( isRunning )
			{
				return m_game->m_UPlayer0Run->GetSpriteTextureAtTime( m_time );
			}
			else
			{
				return m_game->m_UPlayer0Attack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				return m_game->m_UPlayer1Run->GetSpriteTextureAtTime( m_time );
			}
			else
			{
				return m_game->m_UPlayer1Attack->GetSpriteTextureAtTime( m_time );
			}
		}
	}

	if ( m_faction == FACTION_BAD )
	{
		if ( !isSpecial )
		{
			if ( isRunning )
			{
				return m_game->m_UEnemy0Run->GetSpriteTextureAtTime( m_time );
			}
			else
			{
				return m_game->m_UEnemy0Attack->GetSpriteTextureAtTime( m_time );
			}
		}
		else
		{
			if ( isRunning )
			{
				return m_game->m_UEnemy1Run->GetSpriteTextureAtTime( m_time );
			}
			else
			{
				return m_game->m_UEnemy1Attack->GetSpriteTextureAtTime( m_time );
			}
		}
	}

	return nullptr;
}

void UncontrolledHero::PopulateHeroStatesBasedOnUpgrade()
{
	if ( !isSpecial )
	{
		m_health = 5.f;
		m_totalHealth = 5.f;
		m_damage = 1.f;
		m_speed = 5.f;
		m_attackSpeed = 0.9f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
	}

	else
	{
		m_health = 8.f;
		m_totalHealth = 8.f;
		m_damage = 1.f;
		m_speed = 7.f;
		m_attackSpeed = 0.6f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		//m_animationSpeed = 0.65f;
	}
}

void UncontrolledHero::GetHeroToAttack()
{
	if ( m_faction == FACTION_GOOD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_UenemyLane1_1.size(); i++ )
			{
				if ( m_game->m_UenemyLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane1_1[ i ]->m_position , m_game->m_UenemyLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UenemyLane1_1[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_UenemyLane1_2.size(); i++ )
			{
				if ( m_game->m_UenemyLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane1_2[ i ]->m_position , m_game->m_UenemyLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UenemyLane1_2[ i ];
					return;
				}
			}

		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_UenemyLane2_1.size(); i++ )
			{
				if ( m_game->m_UenemyLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UenemyLane2_1[ i ]->m_position , m_game->m_UenemyLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UenemyLane2_1[ i ];
					return;
				}
			}
		}
	}

	if ( m_faction == FACTION_BAD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_UplayerLane1_1.size(); i++ )
			{
				if ( m_game->m_UplayerLane1_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane1_1[ i ]->m_position , m_game->m_UplayerLane1_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UplayerLane1_1[ i ];
					return;
				}
			}
		}

		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_UplayerLane1_2.size(); i++ )
			{
				if ( m_game->m_UplayerLane1_2[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane1_2[ i ]->m_position , m_game->m_UplayerLane1_2[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UplayerLane1_2[ i ];
					return;
				}
			}

		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_UplayerLane2_1.size(); i++ )
			{
				if ( m_game->m_UplayerLane2_1[ i ] == nullptr )
				{
					continue;
				}

				if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_UplayerLane2_1[ i ]->m_position , m_game->m_UplayerLane2_1[ i ]->m_physicsRadius ) )
				{
					m_heroToAttack = m_game->m_UplayerLane2_1[ i ];
					return;
				}
			}
		}
	}
}

