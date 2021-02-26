#include "Game/OccupancyMap.hpp"
#include "Game/Game.hpp"

OccupancyMap::OccupancyMap(Game* game, IntVec2 anchorPoint , IntVec2 dimensions , float maxValue )
{
	m_anchorPoint = anchorPoint;
	m_dimensions = dimensions;
	m_maxValue = maxValue;
	m_currentMinValue = maxValue;
	m_game = game;
	Create();
}

OccupancyMap::OccupancyMap( Game* game , IntVec2 anchorPoint , IntVec2 dimensions )
{
	m_anchorPoint = anchorPoint;
	m_dimensions = dimensions;
	m_game = game;
	CreateForOccMapGame();
}

OccupancyMap::~OccupancyMap()
{

}

void OccupancyMap::Create()
{
	IntVec2 minCoords = IntVec2( m_anchorPoint.x - m_dimensions.x / 2 , m_anchorPoint.y - m_dimensions.y / 2 );
	IntVec2 maxCoords = IntVec2( m_anchorPoint.x + m_dimensions.x / 2 , m_anchorPoint.y + m_dimensions.y / 2 );

	
	int index = 0;
	for ( int i = minCoords.x; i <= maxCoords.x; i++ )
	{
		for ( int j = minCoords.y; j <= maxCoords.y; j++ )
		{
			OccupancyMapNode node;
			//node.hasInfluence = true;
			node.coords = IntVec2( i , j );

			/*int tileIndex = m_game->GetTileIndexForTileCoords( node.coords , true );
			if ( tileIndex > 0 && tileIndex < m_game->m_mainMapTiles.size() && !m_game->m_mainMapTiles[tileIndex].m_isSolid )
			{
				m_nodes.push_back( node );
				std::string s = std::to_string( node.coords.x ) + std::to_string( node.coords.y );
				m_dict[ s ] = index;
				index++;
			}*/

			int tileIndex = m_game->GetTileIndexForOccGame( node.coords );
			if ( tileIndex > 0 && tileIndex < m_game->m_occMapTiles.size() )
			{
				if ( !m_game->m_occMapTiles[ tileIndex ].m_isSolid )
				{
					node.hasInfluence = true;
				}
				m_nodes.push_back( node );
				std::string s = std::to_string( node.coords.x ) + std::to_string( node.coords.y );
				m_dict[ s ] = index;
				index++;
			}
		}
	}

	m_nodes[ GetIndexForCoords( m_anchorPoint ) ].value = m_maxValue;
	m_nodesToIndicesPropogate.push_back( GetIndexForCoords( m_anchorPoint ) );
}

void OccupancyMap::CreateForOccMapGame()
{
	IntVec2 minCoords = IntVec2( m_anchorPoint.x - m_dimensions.x / 2 , m_anchorPoint.y - m_dimensions.y / 2 );
	IntVec2 maxCoords = IntVec2( m_anchorPoint.x + m_dimensions.x / 2 , m_anchorPoint.y + m_dimensions.y / 2 );

	int index = 0;
	for ( int i = minCoords.x; i <= maxCoords.x; i++ )
	{
		for ( int j = minCoords.y; j <= maxCoords.y; j++ )
		{
			OccupancyMapNode node;
			node.coords = IntVec2( i , j );

			int tileIndex = m_game->GetTileIndexForOccGame(node.coords);
			if ( tileIndex > 0 && tileIndex < m_game->m_occMapTiles.size() && !m_game->m_occMapTiles[ tileIndex ].m_isSolid )
			{
				m_nodes.push_back( node );
				std::string s = std::to_string( node.coords.x ) + std::to_string( node.coords.y );
				m_dict[ s ] = index;
				index++;
			}
		}
	}

}

void OccupancyMap::PropgateInfluence()
{
	
	std::vector<int> indicesToPropogate;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].value <= 0.f || !m_nodes[i].hasInfluence )
		{
			continue;
		}

		IntVec2  currentcoords = m_nodes[ i ].coords;
		float currentValue = m_nodes[ i ].value;

		IntVec2 upCoords = IntVec2( currentcoords.x , currentcoords.y + 1 );
		IntVec2 downCoords = IntVec2( currentcoords.x , currentcoords.y - 1 );
		IntVec2 leftCoords = IntVec2( currentcoords.x + 1 , currentcoords.y );
		IntVec2 rightCoords = IntVec2( currentcoords.x - 1 , currentcoords.y );

		int upIndex = GetIndexForCoords( upCoords );
		int downIndex = GetIndexForCoords( downCoords );
		int leftIndex = GetIndexForCoords( leftCoords );
		int rightIndex = GetIndexForCoords( rightCoords );

		bool shouldPropogate = false;

		if ( upIndex > 0 )
		{
			if ( m_nodes[ upIndex ].value ==0.f && m_nodes[upIndex].hasInfluence )
			{
				shouldPropogate = true;
			}
		}

		if ( downIndex > 0 )
		{
			if ( m_nodes[ downIndex ].value  == 0.f && m_nodes[downIndex].hasInfluence )
			{
				shouldPropogate = true;
			}
		}

		if ( leftIndex > 0 )
		{
			if ( m_nodes[ leftIndex ].value ==0.f && m_nodes[leftIndex].hasInfluence )
			{
				shouldPropogate = true;
			}
		}

		if ( rightIndex > 0 )
		{
			if ( m_nodes[ rightIndex ].value == 0.f && m_nodes[rightIndex].hasInfluence )
			{
				shouldPropogate = true;
			}
		}

		if ( shouldPropogate )
		{
			indicesToPropogate.push_back( i );
		}


	}


	for ( int i = 0; i < indicesToPropogate.size(); i++ )
	{
		IntVec2 currentcoords = m_nodes[ indicesToPropogate[ i ] ].coords;
		float currentValue = m_nodes[ indicesToPropogate[ i ] ].value;

		IntVec2 upCoords = IntVec2( currentcoords.x , currentcoords.y + 1 );
		IntVec2 downCoords = IntVec2( currentcoords.x , currentcoords.y - 1 );
		IntVec2 leftCoords = IntVec2( currentcoords.x + 1 , currentcoords.y );
		IntVec2 rightCoords = IntVec2( currentcoords.x - 1 , currentcoords.y );

		int upIndex = GetIndexForCoords( upCoords );
		int downIndex = GetIndexForCoords( downCoords );
		int leftIndex = GetIndexForCoords( leftCoords );
		int rightIndex = GetIndexForCoords( rightCoords );

		if ( upIndex > 0 )
		{
			if ( m_nodes[ upIndex ].value < currentValue - 1.f && m_nodes[upIndex].hasInfluence )
			{
				m_nodes[ upIndex ].value = currentValue-1.f;
			}
		}

		if ( downIndex > 0 )
		{
			if ( m_nodes[downIndex ].value < currentValue - 1.f && m_nodes[downIndex].hasInfluence )
			{
				m_nodes[ downIndex ].value = currentValue-1.f;
			}
		}

		if ( leftIndex > 0 )
		{
			if ( m_nodes[ leftIndex ].value < currentValue - 1.f && m_nodes[leftIndex].hasInfluence )
			{
				m_nodes[ leftIndex ].value = currentValue-1.f;
			}
		}

		if ( rightIndex > 0 )
		{
			if ( m_nodes[ rightIndex ].value < currentValue - 1.f && m_nodes[rightIndex].hasInfluence )
			{
				m_nodes[ rightIndex ].value = currentValue-1.f;
			}
		}
	}


}

IntVec2 OccupancyMap::GetMaxValueCoords()
{
	float max = -100.f;
	int index = 0;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( !m_nodes[ i ].hasInfluence )
		{
			continue;
		}

		if ( m_nodes[ i ].value > max )
		{
			max = m_nodes[ i ].value;
			index = i;
		}
	}

	return m_nodes[ index ].coords;
}

std::vector<IntVec2> OccupancyMap::GetAllMaxValueCoords()
{
	float max = -100.f;
	int index = 0;
	std::vector<IntVec2> toReturn;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( !m_nodes[ i ].hasInfluence )
		{
			continue;
		}

		if ( m_nodes[ i ].value > max )
		{
			max = m_nodes[ i ].value;
			index = i;
		}
	}

	for ( int i = 0; i< m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].value == ( int ) max )
		{
			toReturn.push_back( m_nodes[i].coords );
		}
	}

	return toReturn;
}

float OccupancyMap::GetMaxValue()
{
	float max = -100.f;
	int index = 0;

	for ( int i = 0; i < m_nodes.size(); i++ )
	{
		if ( m_nodes[ i ].value > max )
		{
			max = m_nodes[ i ].value;
			index = i;
		}
	}

	return m_nodes[ index ].value;
}

int OccupancyMap::GetIndexForCoords( IntVec2 coords )
{
	std::string s = std::to_string( coords.x ) + std::to_string( coords.y );

	if ( m_dict.find( s ) != m_dict.end() )
	{
		return m_dict[ s ];
	}
	else
	{
		return -1;
	}
}

void OccupancyMap::SetValue( IntVec2 coords, float value )
{
	int index = GetIndexForCoords( coords );
	if ( index > 0 && index < m_nodes.size() )
	{
		m_nodes[ index ].value = value;
		
		if ( value > 0.f )
		{
			m_nodes[ index ].hasInfluence = true;
		}
	}
}

void OccupancyMap::DebugRender()
{

}
