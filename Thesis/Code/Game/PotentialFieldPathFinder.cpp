#include "Game/PotentialFieldPathFinder.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include <queue>

PotentialFieldPathFinder::PotentialFieldPathFinder( Game* game , std::vector<Tile> tiles )
{
	m_game = game;
	m_tiles = tiles;
}

PotentialFieldPathFinder::~PotentialFieldPathFinder()
{

}

void PotentialFieldPathFinder::GenerateCostFields()
{
	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		if ( m_tiles[ i ].m_isSolid )
		{
			continue;
		}

		std::vector<CostField> costFields;
		for ( int j = 0; j < m_tiles.size(); j++ )
		{
			CostField c;

			if ( i == j )
			{
				c.tileIndex = j;
				c.cost = 0;
				costFields.push_back( c );
				continue;
			}

			if ( m_tiles[ j ].m_isSolid )
			{
				c.tileIndex = j;
				c.cost = 255;
			}
			else
			{
				if ( m_tiles[ j ].m_type == TILE_TYPE_SAND )
				{
					c.tileIndex = j;
					c.cost = 2;
				}

				if ( m_tiles[ j ].m_type == TILE_TYPE_WATER )
				{
					c.tileIndex = j;
					c.cost = 1;
				}

				if ( m_tiles[ j ].m_type == TILE_TYPE_LAVA )
				{
					c.tileIndex = j;
					c.cost = 4;
				}

			}

			costFields.push_back( c );

		}

		m_costFields[ i ] = costFields;
	}
}

void PotentialFieldPathFinder::GenerateIntegrationFields()
{
	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		if ( m_tiles[ i ].m_isSolid )
		{
			continue;
		}

		std::vector<CostField>& costFields = m_costFields[ i ];
		GenerateIntegrationFieldForTile( i , costFields );
	}
}

void PotentialFieldPathFinder::GenerateFlowFields()
{
	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		if ( m_tiles[ i ].m_isSolid )
		{
			continue;
		}

		std::vector<IntegrationField>& intFields = m_intergationFields[ i ];
		GenerateFlowFieldForTile( i , intFields );
	}
}

void PotentialFieldPathFinder::GenerateIntegrationFieldForTile( int tileIndex , std::vector<CostField>& costFields )
{
	std::vector<IntegrationField> intField;

	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		IntegrationField f;
		f.tileIndex = i;
		f.cost = 9999;

		if ( i == tileIndex )
		{
			f.cost = 0;
		}

		intField.push_back( f );
	}

	//std::map<int , int> visitedCells;
	//visitedCells.emplace( tileIndex , tileIndex );

	std::queue<int> cellsToCheck;

	cellsToCheck.push( tileIndex );

	while ( cellsToCheck.size() > 0 )
	{
		int currentIndex = cellsToCheck.front();
		cellsToCheck.pop();

		IntVec2 currentCoords = m_tiles[ currentIndex ].m_tileCoords;
		int currentCost = intField[ currentIndex ].cost;


		int upIndex = -1;
		int downIndex = -1;
		int leftIndex = -1;
		int rightIndex = -1;

		if ( currentCoords.y + 1 < m_mapSize.y );
		{
			upIndex = GetTileIndexForCoords( IntVec2( currentCoords.x , currentCoords.y + 1 ) );
		}

		if ( currentCoords.y - 1 > 0 )
		{
			downIndex = GetTileIndexForCoords( IntVec2( currentCoords.x , currentCoords.y - 1 ) );
		}

		if ( currentCoords.x + 1 < m_mapSize.x )
		{
			rightIndex = GetTileIndexForCoords( IntVec2( currentCoords.x + 1 , currentCoords.y ) );
		}

		if ( currentCoords.x - 1 > 0 )
		{
			leftIndex = GetTileIndexForCoords( IntVec2( currentCoords.x - 1 , currentCoords.y ) );
		}

		if ( upIndex != -1 )
		{
			if ( !m_tiles[ upIndex ].m_isSolid )
			{
				if ( intField[ upIndex ].cost > costFields[ upIndex ].cost + currentCost )
				{
					intField[ upIndex ].cost = costFields[ upIndex ].cost + currentCost;
					cellsToCheck.push( upIndex );
				}
			}
		}

		if ( downIndex != -1 )
		{
			if ( !m_tiles[ downIndex ].m_isSolid )
			{
				if ( intField[ downIndex ].cost > costFields[ downIndex ].cost + currentCost )
				{
					intField[ downIndex ].cost = costFields[ downIndex ].cost + currentCost;
					cellsToCheck.push( downIndex );
				}
			}
		}

		if ( leftIndex != -1 )
		{
			if ( !m_tiles[ leftIndex ].m_isSolid )
			{
				if ( intField[ leftIndex ].cost > costFields[ leftIndex ].cost + currentCost )
				{
					intField[ leftIndex ].cost = costFields[ leftIndex ].cost + currentCost;
					cellsToCheck.push( leftIndex );
				}
			}
		}

		if ( rightIndex != -1 )
		{
			if ( !m_tiles[ rightIndex ].m_isSolid )
			{
				if ( intField[ rightIndex ].cost > costFields[ rightIndex ].cost + currentCost )
				{
					intField[ rightIndex ].cost = costFields[ rightIndex ].cost + currentCost;
					cellsToCheck.push( rightIndex );
				}
			}
		}

	}

	m_intergationFields[ tileIndex ] = intField;
}

void PotentialFieldPathFinder::GenerateFlowFieldForTile( int tildeIndex , std::vector<IntegrationField>& integrationFields )
{
	std::vector<FlowField> flowFields;
	
	for ( int i = 0; i < m_tiles.size(); i++ )
	{
		FlowField flow;
		flow.tileIndex = i;
		flow.flowDirection = Vec2( 0.f , 0.f );

		if (/* m_tiles[ i ].m_isSolid || */i == tildeIndex )
		{
			flowFields.push_back( flow );
			continue;
		}

		IntVec2 currentCoords = m_tiles[ i ].m_tileCoords;
		int topIndex = -1;
		int bottomIndex = -1;
		int rightIndex = -1;
		int leftIndex = -1;
		int topRightIndex = -1;
		int topLeftIndex = -1;
		int bottomRightIndex = -1;
		int bottomLeftIndex = -1;

		std::vector<int> tilesToConsider;

		if ( currentCoords.y + 1 < m_mapSize.y )
		{
			topIndex = GetTileIndexForCoords( IntVec2( currentCoords.x , currentCoords.y + 1 ) );
			tilesToConsider.push_back( topIndex );
		}

		if ( currentCoords.y - 1 > 0 )
		{
			bottomIndex = GetTileIndexForCoords( IntVec2( currentCoords.x , currentCoords.y - 1 ) );
			tilesToConsider.push_back( bottomIndex );
		}

		if ( currentCoords.x + 1 < m_mapSize.x )
		{
			rightIndex = GetTileIndexForCoords( IntVec2( currentCoords.x + 1 , currentCoords.y ) );
			tilesToConsider.push_back( rightIndex );
		}

		if ( currentCoords.x - 1 > 0 )
		{
			leftIndex = GetTileIndexForCoords( IntVec2( currentCoords.x - 1 , currentCoords.y ) );
			tilesToConsider.push_back( leftIndex );
		}

		if ( currentCoords.x + 1 < m_mapSize.x && currentCoords.y + 1 < m_mapSize.y )
		{
			topRightIndex = GetTileIndexForCoords( IntVec2( currentCoords.x + 1 , currentCoords.y + 1 ) );
			tilesToConsider.push_back( topRightIndex );
		}

		if ( currentCoords.x - 1 > 0 && currentCoords.y + 1 < m_mapSize.y )
		{
			topLeftIndex = GetTileIndexForCoords( IntVec2( currentCoords.x - 1 , currentCoords.y + 1 ) );
			tilesToConsider.push_back( topLeftIndex );
		}

		if ( currentCoords.x - 1 > 0 && currentCoords.y - 1 > 0 )
		{
			bottomLeftIndex = GetTileIndexForCoords( IntVec2( currentCoords.x - 1 , currentCoords.y - 1 ) );
			tilesToConsider.push_back( bottomLeftIndex );
		}

		if ( currentCoords.x + 1 < m_mapSize.x && currentCoords.y - 1 > 0 )
		{
			bottomRightIndex = GetTileIndexForCoords( IntVec2( currentCoords.x + 1 , currentCoords.y - 1 ) );
			tilesToConsider.push_back( bottomRightIndex );
		}

		int min = integrationFields[i].cost;
		int index = -1;
		for ( int j = 0; j < tilesToConsider.size(); j++ )
		{
			int intFieldValue = integrationFields[ tilesToConsider[ j ] ].cost;

			if ( intFieldValue < min )
			{
				min = intFieldValue;
				index = tilesToConsider[j];
			}
		}

		if ( index == -1 )
		{
			flow.flowDirection = Vec2( 0.f , 0.f );
			flowFields.push_back( flow );
			continue;
		}

		IntVec2 dirCoords = m_tiles[ index ].m_tileCoords;
		Vec2 vec1 = Vec2( currentCoords.x + 0.5f , currentCoords.y + 0.5f );
		Vec2 vec2 = Vec2( dirCoords.x + 0.5f , dirCoords.y + 0.5f );
		
		flow.flowDirection = ( vec2 - vec1 ).GetNormalized();

		flowFields.push_back( flow );

	}

	m_flowFields[ tildeIndex ] = flowFields;
}

void PotentialFieldPathFinder::DebugRenderCostFields( int index )
{
	std::vector<CostField>& costFields = m_costFields[ index ];
	std::vector<Vertex_PCU> costFieldValues;

	for ( int i = 0; i < costFields.size(); i++ )
	{
		int tileIndex = costFields[ i ].tileIndex;

		AABB2 aabb = AABB2( m_tiles[ tileIndex ].m_tileCoords.x , m_tiles[ tileIndex ].m_tileCoords.y , m_tiles[ tileIndex ].m_tileCoords.x+1 , m_tiles[ tileIndex ].m_tileCoords.y+1 );
		m_game->m_font->AddVertsForTextInBox2D( costFieldValues , aabb , 0.4f , std::to_string( costFields[ i ].cost ) , Rgba8( 0 , 0 , 0 , 255 ) , 0.5f , Vec2(0.1f,0.1f) );
	}

	g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( costFieldValues );
	g_theRenderer->BindTexture( nullptr );
	

}

void PotentialFieldPathFinder::DebugRenderIntegrationFields( int index )
{
	std::vector<IntegrationField>& costFields = m_intergationFields[ index ];
	std::vector<Vertex_PCU> costFieldValues;

	for ( int i = 0; i < costFields.size(); i++ )
	{
		int tileIndex = costFields[ i ].tileIndex;

		AABB2 aabb = AABB2( m_tiles[ tileIndex ].m_tileCoords.x , m_tiles[ tileIndex ].m_tileCoords.y , m_tiles[ tileIndex ].m_tileCoords.x + 1 , m_tiles[ tileIndex ].m_tileCoords.y + 1 );
		m_game->m_font->AddVertsForTextInBox2D( costFieldValues , aabb , 0.5f , std::to_string( costFields[ i ].cost ) , Rgba8( 0 , 0 , 0 , 255 ) , 0.5f , Vec2( 0.f , 0.f ) );
	}

	g_theRenderer->BindTexture( m_game->m_font->GetTexture() );
	g_theRenderer->DrawVertexArray( costFieldValues );
	g_theRenderer->BindTexture( nullptr );

}

void PotentialFieldPathFinder::DebugRenderFlowFields( int index )
{
	std::vector<FlowField>& flowFields = m_flowFields[ index ];
	//std::vector<Vertex_PCU> flowFieldValues;

	for ( int i = 0; i < flowFields.size(); i++ )
	{
		int tileIndex = flowFields[ i ].tileIndex;

		AABB2 aabb = AABB2( m_tiles[ tileIndex ].m_tileCoords.x , m_tiles[ tileIndex ].m_tileCoords.y , m_tiles[ tileIndex ].m_tileCoords.x + 1 , m_tiles[ tileIndex ].m_tileCoords.y + 1 );
		Vec2 startPoint = aabb.GetCenter() + Vec2(0.f,-0.4f);
		Vec2 endPoint = startPoint + flowFields[ i ].flowDirection * 0.8f;

		g_theRenderer->BindTexture( nullptr );
		g_theRenderer->DrawLine( startPoint , endPoint , Rgba8( 0 , 0 , 0 , 255 ) , 0.05f );
		g_theRenderer->DrawDisc( endPoint , 0.1f , Rgba8(0,0,0,255) );
	}
}

int PotentialFieldPathFinder::GetTileIndexForCoords( IntVec2 coords )
{
	return coords.x + ( m_mapSize.x * coords.y );
}
