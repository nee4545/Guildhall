#include "Game/InfluenceMap.hpp"
#include "Game/Game.hpp"
#include "Game/OccupancyMap.hpp"
#include "Engine/Math/MathUtils.hpp"

InfluenceMap::InfluenceMap( Game* game , IntVec2 startPos , IntVec2 dimesions , float startValue, float fallOff )
{
	m_game = game;
	m_startPosition = startPos;
	m_dimesions = dimesions;
	m_startValue = startValue;
	m_fallOffPerTile = fallOff;
}

InfluenceMap::InfluenceMap( Game* game )
{
	m_game = game;
}

InfluenceMap::~InfluenceMap()
{
	ClearInfluence();
}

void InfluenceMap::Create()
{
	IntVec2 minCoords = IntVec2( m_startPosition.x - m_dimesions.x / 2 , m_startPosition.y - m_dimesions.y / 2 );
	IntVec2 maxCoords = IntVec2( m_startPosition.x + m_dimesions.x / 2 , m_startPosition.y + m_dimesions.y / 2 );

	int index = 0;
	for ( int i = minCoords.x; i <= maxCoords.x; i++ )
	{
		for ( int j = minCoords.y; j <= maxCoords.y; j++ )
		{
			InfluenceMapNode node;
			node.coords = IntVec2( i , j );

			float manDistance = m_game->GetManhattanDistance( node.coords , m_startPosition );
			node.influenceValue = m_startValue - manDistance*m_fallOffPerTile;
			std::string s1 = std::to_string( node.coords.x );
			std::string s2 = std::to_string( node.coords.y );
			s2[0] = toupper( s2[ 0 ] );
			std::string s = s1 + s2;
			m_dict[ s ] = index;
			index++;

			m_nodes.push_back( node );
			
		}
	}
}



void InfluenceMap::ClearInfluence()
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		m_nodes[ i ].influenceValue = 0.f;
	}
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

int InfluenceMap::GetIndexForCoords( IntVec2 coords )
{
	std::string s1 = std::to_string( coords.x );
	std::string s2 = std::to_string( coords.y );
	s2[0] = toupper( s2[ 0 ] );
	std::string s = s1 + s2;

	if ( m_dict.find( s ) != m_dict.end() )
	{
		return m_dict[ s ];
	}
	else
	{
		return -1;
	}
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

IntVec2 InfluenceMap::GetCoordsWithValue( float value )
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( ( int ) m_nodes[ i ].influenceValue - ( int ) value == 0)
		{
			return m_nodes[ i ].coords;
		}
	}

	return IntVec2( -1 , -1 );
}

std::vector<IntVec2> InfluenceMap::GetAllCoordsWithValue( float value )
{
	std::vector<IntVec2> toReturn;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( ( int ) m_nodes[ i ].influenceValue - ( int ) value == 0 )
		{
			toReturn.push_back( m_nodes[ i ].coords );
		}
	}

	return toReturn;

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

void InfluenceMap::AddFromMap( InfluenceMap* mapToAdd )
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		for ( int j = 0; j < mapToAdd->m_nodes.size(); j++ )
		{
			if ( m_nodes[ i ].coords == mapToAdd->m_nodes[ j ].coords )
			{
				m_nodes[ i ].influenceValue += mapToAdd->m_nodes[ j ].influenceValue;
				break;
			}
		}
	}
}


void InfluenceMap::AddFromOccupancyMap( OccupancyMap* mapToAdd )
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		for ( int j = 0; j < mapToAdd->m_nodes.size(); j++ )
		{
			if ( m_nodes[ i ].coords == mapToAdd->m_nodes[ j ].coords )
			{
				m_nodes[ i ].influenceValue += mapToAdd->m_nodes[ j ].value;
				break;
			}
		}
	}
}

void InfluenceMap::SubractFromMap( InfluenceMap* mapToSubract )
{
	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		for ( int j = 0; j < mapToSubract->m_nodes.size(); j++ )
		{
			if ( m_nodes[ i ].coords == mapToSubract->m_nodes[ j ].coords )
			{
				m_nodes[ i ].influenceValue -= mapToSubract->m_nodes[ j ].influenceValue;
				break;
			}
		}
	}
}
