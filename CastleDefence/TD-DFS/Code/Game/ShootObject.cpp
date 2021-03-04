#include "Game/ShootObject.hpp"
#include "Game/RangeHero.hpp"
#include "Game/Hero.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"

ShootObject::ShootObject( Game* game , Vec2 position ,Texture* tex, Faction faction /*= FACTION_GOOD */ , Lanes lane ) :Entity( game , position )
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
	m_texture = tex;
	m_lane = lane;
	m_physicsRadius = 2.f;
}

void ShootObject::Update( float deltaSeconds )
{
	if ( m_faction == FACTION_GOOD )
	{
		m_position.x += m_speed * deltaSeconds;
	}
	else
	{
		m_position.x -= m_speed * deltaSeconds;
	}

	DetectCollission();

	if ( m_position.x >= 295.f || m_position.x <= 20.f )
	{
		Die();
	}

}

void ShootObject::Render()
{
	g_theRenderer->TransformVertexArray( 6 , m_vertices , 5.f , 0.f , m_position );
	g_theRenderer->BindTexture( m_texture );
	
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

void ShootObject::Die()
{
	m_isGarbage = true;
}

void ShootObject::DebugRender()
{

}

void ShootObject::ResetVertices()
{
	for ( int index = 0; index < 6; index++ )
	{
		m_vertices[ index ] = m_vertices_original[ index ];
	}
}

void ShootObject::DetectCollission()
{
	if ( m_faction == FACTION_GOOD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane1_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_1[ i ]->m_position , m_game->m_enemyRangeHerosLane1_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyRangeHerosLane1_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane1_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_1[ i ]->m_position , m_game->m_enemyMeleeHerosLane1_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyMeleeHerosLane1_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_enemyRangeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane1_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane1_2[ i ]->m_position , m_game->m_enemyRangeHerosLane1_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyRangeHerosLane1_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane1_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane1_2[ i ]->m_position , m_game->m_enemyMeleeHerosLane1_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyMeleeHerosLane1_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane2_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_1[ i ]->m_position , m_game->m_enemyRangeHerosLane2_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyRangeHerosLane2_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane2_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_1[ i ]->m_position , m_game->m_enemyMeleeHerosLane2_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyMeleeHerosLane2_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE2_2 )
		{
			for ( int i = 0; i < m_game->m_enemyRangeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_enemyRangeHerosLane2_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyRangeHerosLane2_2[ i ]->m_position , m_game->m_enemyRangeHerosLane2_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyRangeHerosLane2_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_enemyMeleeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_enemyMeleeHerosLane2_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_enemyMeleeHerosLane2_2[ i ]->m_position , m_game->m_enemyMeleeHerosLane2_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_enemyMeleeHerosLane2_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

	}

	if ( m_faction == FACTION_BAD )
	{
		if ( m_lane == LANE1_1 )
		{
			for ( int i = 0; i < m_game->m_playerRangeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane1_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_1[ i ]->m_position , m_game->m_playerRangeHerosLane1_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerRangeHerosLane1_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_1.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane1_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_1[ i ]->m_position , m_game->m_playerMeleeHerosLane1_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerMeleeHerosLane1_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE1_2 )
		{
			for ( int i = 0; i < m_game->m_playerRangeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane1_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane1_2[ i ]->m_position , m_game->m_playerRangeHerosLane1_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerRangeHerosLane1_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_playerMeleeHerosLane1_2.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane1_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane1_2[ i ]->m_position , m_game->m_playerMeleeHerosLane1_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerMeleeHerosLane1_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE2_1 )
		{
			for ( int i = 0; i < m_game->m_playerRangeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane2_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_1[ i ]->m_position , m_game->m_playerRangeHerosLane2_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerRangeHerosLane2_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_1.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane2_1[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_1[ i ]->m_position , m_game->m_playerMeleeHerosLane2_1[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerMeleeHerosLane2_1[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

		if ( m_lane == LANE2_2 )
		{
			for ( int i = 0; i < m_game->m_playerRangeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_playerRangeHerosLane2_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerRangeHerosLane2_2[ i ]->m_position , m_game->m_playerRangeHerosLane2_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerRangeHerosLane2_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}

			for ( int i = 0; i < m_game->m_playerMeleeHerosLane2_2.size(); i++ )
			{
				if ( m_game->m_playerMeleeHerosLane2_2[ i ] != nullptr )
				{
					if ( DoDiscsOverlap( m_position , m_physicsRadius , m_game->m_playerMeleeHerosLane2_2[ i ]->m_position , m_game->m_playerMeleeHerosLane2_2[ i ]->m_physicsRadius ) )
					{
						m_game->m_playerMeleeHerosLane2_2[ i ]->TakeDamage( 0.5f );
						Die();
						return;
					}
				}
			}
		}

	}
}


