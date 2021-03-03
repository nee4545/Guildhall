#include "Game/InfluenceMap.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/MathUtils.hpp"

InfluenceMap::InfluenceMap( Game* game , IntVec2 startPos , IntVec2 dimesions , int startValue, int fallOff )
{
	m_game = game;
	m_startPosition = startPos;
	m_dimesions = dimesions;
	m_startValue = startValue;
	m_fallOffPerTile = fallOff;
}

InfluenceMap::~InfluenceMap()
{
	ClearInfluence();
}

void InfluenceMap::Create()
{
	IntVec2 minCoords = IntVec2( m_startPosition.x - m_dimesions.x / 2 , m_startPosition.y - m_dimesions.y / 2 );
	IntVec2 maxCoords = IntVec2( m_startPosition.x + m_dimesions.x / 2 , m_startPosition.y + m_dimesions.y / 2 );

	for ( int i = minCoords.x; i <= maxCoords.x; i++ )
	{
		for ( int j = minCoords.y; j <= maxCoords.y; j++ )
		{
			InfluenceMapNode node;
			node.coords = IntVec2( i , j );

			float manDistance = m_game->GetManhattanDistance( node.coords , m_startPosition );
			node.influenceValue = m_startValue - manDistance*m_fallOffPerTile;

			if ( m_startValue > 0 )
			{
				node.influenceValue = (int)Clamp( node.influenceValue , 0.f , m_startValue );
			}

			if ( node.influenceValue != 0 )
			{
				m_nodes.push_back( node );
			}
		}
	}

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords.x > 0 && m_nodes[ i ].coords.x < m_game->m_mapSize.x && m_nodes[ i ].coords.y>0 && m_nodes[ i ].coords.y < m_game->m_mapSize.y )
		{
			if ( !m_game->m_tiles[ m_game->GetTileIndexForTileCoords( m_nodes[ i ].coords) ].m_isSolid )
			{
				m_game->m_tiles[ m_game->GetTileIndexForTileCoords( m_nodes[ i ].coords ) ].m_influenceValue = m_nodes[ i ].influenceValue;
			}
		}
	}


	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords.x > 0 && m_nodes[ i ].coords.x < m_game->m_mainMapSize.x && m_nodes[ i ].coords.y>0 && m_nodes[ i ].coords.y < m_game->m_mainMapSize.y )
		{
			if ( !m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( m_nodes[ i ].coords , true) ].m_isSolid )
			{
				m_game->m_mainMapTiles[ m_game->GetTileIndexForTileCoords( m_nodes[ i ].coords, true ) ].m_influenceValue = m_nodes[ i ].influenceValue;
			}
		}
	}
}

void InfluenceMap::ClearInfluence()
{

}

IntVec2 InfluenceMap::GetMaxValueCoords()
{
	int max = -100;
	int index = 0;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].influenceValue > max )
		{
			max = m_nodes[ i ].influenceValue;
			index = i;
		}
	}

	return m_nodes[ index ].coords;
}

void InfluenceMap::ClearInfluenceAtNode( IntVec2 coords )
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords == coords )
		{
			m_nodes[ i ].influenceValue = 0;
			break;
		}
	}

}

void InfluenceMap::Correct()
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].coords.x<0 || m_nodes[ i ].coords.y<0 || m_nodes[ i ].coords.x > m_game->m_mainMapSize.x || m_nodes[ i ].coords.y >m_game->m_mainMapSize.y )
		{

		}
	}
}
