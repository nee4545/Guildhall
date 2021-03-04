
#include "Game/RangeHero.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefTex.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"


RangeHero::RangeHero( Game* game , Vec2 position , Faction faction , Lanes lane , Upgrade upgrade ) :Entity( game , position )
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
	m_upgrade = upgrade;
	m_lane = lane;
	PopulateHeroStatesBasedOnUpgrade();

	m_healbarBase = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_bg.png" );
	m_healthbar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_2.png" );

}

void RangeHero::Update( float deltaSeconds )
{

	if ( m_isGarbage )
	{
		return;
	}

	if ( m_faction == FACTION_GOOD )
	{
		if ( m_position.x >= 290.f )
		{
			m_game->ReduceHealth( 10.f , true );
			Die();
		}
	}
	else
	{
		if ( m_position.x <= 20.f )
		{
			m_game->ReduceHealth( 10.f , false );
			Die();
		}
	}

	m_time += deltaSeconds;

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

	if ( m_attackTimer->HasElapsed() )
	{
		m_attackTimer->Reset();
		m_attackAnimationTimer = 1.f;
		isAttacking = true;
		isRunning = false;
		m_game->CreateShootObjects( m_faction , m_position + Vec2( 1.f , 0.f ) , GetShootTexture() , m_lane );
	}
	else
	{
		if ( m_attackAnimationTimer <= 0.1f )
		{
			isAttacking = false;
			isRunning = true;
		}
	}

	m_attackAnimationTimer -= deltaSeconds;

	
	if ( m_attackAnimationTimer <= 0.f )
	{
		ResetAnimationTimer();
	}

}

void RangeHero::Render()
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

void RangeHero::Die()
{
	m_isGarbage = true;

	switch ( m_upgrade )
	{
	case UPGRADE_NONE:
	{
		m_game->m_playerCoins += RANGE_UPGRADE0_DEATH_COINS;
		break;
	}
	case UPGRADE_SMALL:
	{
		m_game->m_playerCoins += RANGE_UPGRADE1_DEATH_COINS;
		break;
	}
	case UPGRADE_MEDIUM:
	{
		m_game->m_playerCoins += RANGE_UPGRADE2_DEATH_COINS;
		break;
	}
	case UPGRADE_LARGE:
	{
		m_game->m_playerCoins += RANGE_UPGRADE3_DEATH_COINS;
		break;
	}
	default:
		break;
	}

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
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_health = 3.f;
		m_totalHealth = 3.f;
		m_damage = 1.f;
		m_speed = 10.f;
		m_attackSpeed = 3.5f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_attackAnimationTimer = 1.f;
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_health = 5.f;
		m_totalHealth = 5.f;
		m_damage = 1.f;
		m_speed = 8.f;
		m_attackSpeed = 3.f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_attackAnimationTimer = 1.f;
	}

	if ( m_upgrade == UPGRADE_MEDIUM )
	{
		m_health = 6.f;
		m_totalHealth = 6.f;
		m_damage = 1.f;
		m_speed = 5.f;
		m_attackSpeed = 2.5f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_attackAnimationTimer = 0.8f;
	}

	if ( m_upgrade == UPGRADE_LARGE )
	{
		m_health = 9.f;
		m_totalHealth = 9.f;
		m_damage = 1.f;
		m_speed = 4.f;
		m_attackSpeed = 1.8f;
		m_attackTimer = new Timer();
		Clock* c = new Clock();
		m_attackTimer->SetSeconds( c , m_attackSpeed );
		m_attackAnimationTimer = 0.6f;
	}

}

void RangeHero::Attack()
{

}

void RangeHero::DetectHeroToAttack()
{

}

void RangeHero::ResetAnimationTimer()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_attackAnimationTimer = 1.f;
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_attackAnimationTimer = 1.f;
	}

	if ( m_upgrade == UPGRADE_MEDIUM )
	{
		m_attackAnimationTimer = 0.8f;
	}

	if ( m_upgrade == UPGRADE_LARGE )
	{
		m_attackAnimationTimer = 0.6f;
	}
}

Texture* RangeHero::GetShootTexture()
{
	if ( m_faction == FACTION_GOOD )
	{
		switch ( m_upgrade )
		{
		case UPGRADE_NONE:
		{
			return m_game->m_RangerHero0PlayerBulletTex;
			break;
		}
		case UPGRADE_SMALL:
		{
			return m_game->m_RangerHero1PlayerBulletTex;
			break;
		}
		case UPGRADE_MEDIUM:
		{
			return m_game->m_RangerHero2PlayerBulletTex;
			break;
		}
		case UPGRADE_LARGE:
		{
			return m_game->m_RangerHero3PlayerBulletTex;
			break;
		}
		default:
			return nullptr;
		}
	}

	if ( m_faction == FACTION_BAD )
	{
		switch ( m_upgrade )
		{
		case UPGRADE_NONE:
		{
			return m_game->m_RangerHero0EnemyBulletTex;
			break;
		}
		case UPGRADE_SMALL:
		{
			return m_game->m_RangerHero1EnemyBulletTex;
			break;
		}
		case UPGRADE_MEDIUM:
		{
			return m_game->m_RangerHero2EnemyBulletTex;
			break;
		}
		case UPGRADE_LARGE:
		{
			return m_game->m_RangerHero3EnemyBulletTex;
			break;
		}
		default:
			return nullptr;
		}
	}

	return nullptr;
}

