#include "Game/OccAI.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/OccupancyMap.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

OccAI::OccAI()
{
	m_helper = new MovingHelper();

	m_animClock = new Clock();
	m_occupancyUpdateTimer = new Timer();

	m_occupancyUpdateTimer->SetSeconds( 0.38f );
}

OccAI::~OccAI()
{

}

void OccAI::Update( float deltaseconds )
{
	UpdateBehavior();

	if ( m_type == OCC_AI_SHARED_MAP )
	{
		if ( m_state == OCC_CHASE_PLAYER )
		{
			Vec2 dir = ( m_game->m_player->m_position - m_position ).GetNormalized();
			m_position += dir * 2.f * deltaseconds;

			m_lastSeenPosition = m_game->m_player->m_position;
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			OccupancyMap* sharedMap= m_game->m_player->m_aiSharedMap;
			if ( sharedMap == nullptr )
			{
				m_game->m_player->CreateMap();
			}
			else
			{
				for ( int i = 0; i < sharedMap->m_nodes.size(); i++ )
				{
					if ( sharedMap->m_nodes[ i ].value <= 0.f )
					{
						continue;
					}

					IntVec2 coords = sharedMap->m_nodes[ i ].coords;
					if ( !m_game->RayCastInOccGame( m_position , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetNormalized() , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetLength() ).didImpact )
					{
						sharedMap->SetValue( coords , 0.f );

						// If we see our path's end location, find a new path
						if ( coords == m_currentMaxCoords )
						{
							m_helper->Reset();
						}
					}
				}

				if ( !m_helper->DoesHavePath() )
				{
					m_nextMovePos = m_position;
					std::vector<IntVec2> allCoords = sharedMap->GetAllMaxValueCoords();
					IntVec2 maxCoords = allCoords[ 0 ];
					m_currentMaxCoords = maxCoords;

					if ( allCoords.size() > 1 )
					{
						for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
						{
							if ( m_game->m_occAIs[ i ] != this )
							{
								if ( m_game->m_occAIs[ i ]->m_currentMaxCoords == maxCoords )
								{
									maxCoords = allCoords[ 1 ];
									m_currentMaxCoords = maxCoords;
								}
							}
						}
					}


					m_game->GetPathInOccupancyGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path );
					m_helper->currentIndex = m_helper->path.size() - 1;
				}
				else if ( ( m_nextMovePos - m_position ).GetLengthSquared() <= 0.1f )
				{
					tileIndex = m_helper->GetNextMove();
					if ( tileIndex > 0 )
					{
						m_nextMovePos = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
					}
				}
				else
				{
					Vec2 moveVec = ( m_nextMovePos - m_position ).GetNormalized();
					m_position += moveVec * 4.5f * deltaseconds;

					/*float maxValue = sharedMap->GetMaxValue();
					int index = sharedMap->GetIndexForCoords( m_currentMaxCoords );
					if ( index > -1 )
					{
						if ( (int)sharedMap->m_nodes[index].value != (int)maxValue )
						{
							m_helper->Reset();
							tileIndex = -1;
						}
					}*/
				}

			
			}
		}
	}

	
	if ( m_type == OCC_AI_SINGLE_MAP )
	{
		if ( m_state == OCC_CHASE_PLAYER )
		{
			Vec2 dir = ( m_game->m_player->m_position - m_position ).GetNormalized();
			m_position += dir * 2.f * deltaseconds;

			m_lastSeenPosition = m_game->m_player->m_position;
			m_lastSeenSet = true;

			if ( m_occupancyMap != nullptr )
			{
				delete m_occupancyMap;
				m_occupancyMap = nullptr;
			}

			if ( initialUpdateDone )
			{
				initialUpdateDone = false;
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{

			if ( m_occupancyMap != nullptr )
			{
				if ( m_occupancyUpdateTimer->HasElapsed() )
				{
					m_occupancyMap->PropgateInfluence();
					m_occupancyUpdateTimer->Reset();
				}

				for ( int i = 0; i < m_occupancyMap->m_nodes.size(); i++ )
				{
					if ( m_occupancyMap->m_nodes[ i ].value <= 0.f )
					{
						continue;
					}

					IntVec2 coords = m_occupancyMap->m_nodes[ i ].coords;
					if ( !m_game->RayCastInOccGame( m_position , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetNormalized() , ( Vec2( coords.x + 0.5f , coords.y + 0.5f ) - m_position ).GetLength() ).didImpact )
					{
						m_occupancyMap->SetValue( coords , 0.f );

						// If we see our path's end location, find a new path
						/*if ( coords == m_currentMaxCoords )
						{
							m_helper->Reset();
						}*/
					}
				}
			}

			if ( m_lastSeenSet )
			{
				m_helper->path.clear();
				m_helper->Reset();
				Vec2 dir = ( m_lastSeenPosition - m_position ).GetNormalized();
				m_position += dir * 5.f * deltaseconds;

				if ( ( m_position - m_lastSeenPosition ).GetLengthSquared() <= 0.5f )
				{
					m_lastSeenSet = false;
				}

				if ( m_occupancyMap == nullptr )
				{
					m_occupancyMap = new OccupancyMap( m_game , IntVec2( m_lastSeenPosition.x , m_lastSeenPosition.y ) , IntVec2( 40 , 30 ) , 40.f );
					//m_occupancyMap->SetValue( IntVec2( ( int ) m_lastSeenPosition.x , ( int ) m_lastSeenPosition.y ) , 50.f );
				}

				if ( !initialUpdateDone )
				{
					m_occupancyMap->PropgateInfluence();
					m_occupancyMap->PropgateInfluence();
					initialUpdateDone = true;
				}

				m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );

			}
			else if ( m_occupancyMap != nullptr )
			{

				m_occupancyMap->SetValue( IntVec2( m_position.x , m_position.y ) , 0.f );

				if ( !m_helper->DoesHavePath() )
				{
					m_nextMovePos = m_position;
					IntVec2 maxCoords = m_occupancyMap->GetMaxValueCoords();
					m_currentMaxCoords = maxCoords;
					m_game->GetPathInOccupancyGame( m_position , Vec2( maxCoords.x , maxCoords.y ) , m_helper->path,false,true );
					m_helper->currentIndex = m_helper->path.size() - 1;
				}
				else if ( ( m_nextMovePos - m_position ).GetLengthSquared() <= 0.1f )
				{
					tileIndex = m_helper->GetNextMove();
					if ( tileIndex > 0 )
					{
						m_nextMovePos = Vec2( m_game->m_occMapTiles[ tileIndex ].m_tileCoords.x + 0.5f , m_game->m_occMapTiles[ tileIndex ].m_tileCoords.y + 0.5f );
					}

				}
				else
				{
					Vec2 moveVec = ( m_nextMovePos - m_position ).GetNormalized();
					m_position += moveVec * 5.2f * deltaseconds;
				}
			}
		}
	}
}

void OccAI::Render()
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawDisc( m_position , 0.5f , Rgba8( 0 , 0 , 100 , 255 ) );

	DebugRender();
}

void OccAI::DebugRender()
{
	if ( m_occupancyMap != nullptr )
	{
		std::vector<Vertex_PCU> verts;
		std::vector<Vertex_PCU> fontVerts;
		float maxValue = m_occupancyMap->GetMaxValue();

		for ( int i = 0; i < m_occupancyMap->m_nodes.size(); i++ )
		{
			if ( m_occupancyMap->m_nodes[ i ].value > 0.f )
			{
				Rgba8 baseColor = Rgba8( 255 , 0 , 0 , 0 );
				baseColor.a = RangeMapFloat( 0 , maxValue , 0 , 100 , m_occupancyMap->m_nodes[ i ].value );

				AABB2 aabb = AABB2( m_occupancyMap->m_nodes[ i ].coords.x , m_occupancyMap->m_nodes[ i ].coords.y , m_occupancyMap->m_nodes[ i ].coords.x + 1 , m_occupancyMap->m_nodes[ i ].coords.y + 1 );
				AppendAABB2( verts , aabb , baseColor );

				m_game->m_font->AddVertsForTextInBox2D( fontVerts , aabb , 0.4f , std::to_string( (int)m_occupancyMap->m_nodes[ i ].value ) , Rgba8( 0 , 0 , 0 , 255 ) , 0.5f , Vec2( 0.2f , 0.2f ) );
			}


		}
		g_theRenderer->DrawVertexArray( verts );
		g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
		g_theRenderer->DrawVertexArray( fontVerts );
		g_theRenderer->BindTexture( nullptr );
	}


	std::vector<Vertex_PCU> pathVerts;
	std::vector<int>& pathIndices = m_helper->path;

	for ( int i = 0; i < pathIndices.size(); i++ )
	{
		AABB2 aabb = AABB2( m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.x , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.y , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.x + 1 , m_game->m_occMapTiles[ pathIndices[ i ] ].m_tileCoords.y + 1 );
		AppendAABB2( pathVerts , aabb , Rgba8( 0 , 0 , 255 , 70 ) );
	}

	g_theRenderer->DrawVertexArray( pathVerts );
}

void OccAI::Die()
{
	m_isGarbage = true;
}

void OccAI::SetVerticesBasedOnAspect( Vec2 aspect )
{

}

void OccAI::CheckPlayerInSight()
{

}

void OccAI::UpdateBehavior()
{

	if ( m_type == OCC_AI_SHARED_MAP )
	{
		if ( m_state == OCC_IDLE )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}
		if ( m_state == OCC_CHASE_PLAYER )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( ( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_SEARCH_MAP;
				for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
				{
					if ( m_game->m_occAIs[ i ] != this )
					{
						if ( m_game->m_occAIs[ i ]->m_type == OCC_AI_SHARED_MAP )
						{
							m_game->m_occAIs[ i ]->m_state = OCC_SEARCH_MAP;
						}
					}
				}
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;

				m_game->m_player->DeleteMap();
				if ( m_helper->DoesHavePath() )
				{
					m_helper->Reset();
				}

				for ( int i = 0; i < m_game->m_occAIs.size(); i++ )
				{
					if ( m_game->m_occAIs[ i ] != this )
					{
						if ( m_game->m_occAIs[ i ]->m_type == OCC_AI_SHARED_MAP )
						{
							m_game->m_occAIs[ i ]->m_state = OCC_IDLE;
						}
					}
				}

			}
		}
	}


	if ( m_type == OCC_AI_SINGLE_MAP )
	{
		if ( m_state == OCC_IDLE )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}

		if ( m_state == OCC_CHASE_PLAYER )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( ( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_SEARCH_MAP;
			}
		}

		if ( m_state == OCC_SEARCH_MAP )
		{
			Vec2 direction = ( m_game->m_player->m_position - m_position ).GetNormalized();
			float distance = ( m_game->m_player->m_position - m_position ).GetLength();
			if ( !( m_game->RayCastInOccGame( m_position , direction , distance ).didImpact ) )
			{
				m_state = OCC_CHASE_PLAYER;
			}
		}
	}
}
