#include "Game/Tower.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"

Tower::Tower( Game* game , Vec2 position , Faction faction /*= FACTION_GOOD */, Upgrade upg ) :Entity( game , position )
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

	m_upgrade = upg;
	m_attackClock = new Clock();
	m_attackTimer = new Timer();
	m_rotationTimer = new Timer();
	m_faction = faction;
	m_orientationDegrees = 90.f;

	PopulateStats();

	m_healbarBase = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_bg.png" );
	m_healthbar = g_theRenderer->GetOrCreateTextureFromFile( "Data/GameAssets/GUI/PNG/load_bar/load_bar_2.png" );
	
}

void Tower::Update( float deltaSeconds )
{

	if ( m_rotationTimer->HasElapsed() )
	{
		rotateClockWise = !rotateClockWise;
		m_rotationTimer->Reset();
	}
	
	if ( rotateClockWise )
	{
		m_orientationDegrees -= 45.f * deltaSeconds;
	}
	else
	{
		m_orientationDegrees += 45.f * deltaSeconds;
	}

	
	if ( m_attackTimer->HasElapsed() )
	{
		m_attackTimer->Reset();
		m_game->SpawnToweBullet( m_faction , m_upgrade , m_position , m_orientationDegrees-90.f , true );
	}
	

}

void Tower::Render()
{
	g_theRenderer->BindTexture( m_game->m_towerBase );
	g_theRenderer->DrawAABB2D( m_towerBase , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->BindTexture( m_game->m_tower );
	g_theRenderer->DrawAABB2D( m_tower , Rgba8( 255 , 255 , 255 , 255 ) );
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 12.f , m_orientationDegrees , m_position );
	g_theRenderer->BindTexture( m_game->m_towerTurret );
	g_theRenderer->DrawVertexArray( 6 , m_vertices );
	ResetVertices();

	AABB2 healthBar = AABB2( m_position.x - 3.f , m_position.y + 2.f , m_position.x + 3.f , m_position.y + 4.5f );
	AABB2 actualHealth = healthBar.GetPercentageOfAABB( 0.9f );
	actualHealth.CarveBoxOffLeft( ( m_health / 20.f ) );

	g_theRenderer->BindTexture( m_healbarBase );
	g_theRenderer->DrawAABB2D( healthBar , Rgba8( 255 , 255 , 255 , 125 ) );
	g_theRenderer->BindTexture( m_healthbar );
	g_theRenderer->DrawAABB2D( actualHealth , Rgba8( 255 , 255 , 255 , 125 ) );
	g_theRenderer->BindTexture( nullptr );
}

void Tower::Die()
{
	m_isGarbage = true;
}

void Tower::TakeDamage( float damageAmt )
{
	m_health -= damageAmt;
	
	if ( m_health <= 0.f )
	{
		Die();
	}
}

void Tower::DebugRender()
{

}

void Tower::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

void Tower::PopulateStats()
{
	if ( m_upgrade == UPGRADE_NONE )
	{
		m_rotationTimer->SetSeconds( m_attackClock , 4.f );
		m_attackTimer->SetSeconds( m_attackClock , 1.7f );
	}

	if ( m_upgrade == UPGRADE_SMALL )
	{
		m_rotationTimer->SetSeconds( m_attackClock , 4.f );
		m_attackTimer->SetSeconds( m_attackClock , 2.f );
	}

	if ( m_upgrade == UPGRADE_MEDIUM )
	{
		m_rotationTimer->SetSeconds( m_attackClock , 4.f );
		m_attackTimer->SetSeconds( m_attackClock , 1.4f );
	}

	if ( m_upgrade == UPGRADE_LARGE )
	{
		m_rotationTimer->SetSeconds( m_attackClock , 4.f );
		m_attackTimer->SetSeconds( m_attackClock , 1.f );
	}
}

void Tower::UpgradeTower( Upgrade u )
{

}
