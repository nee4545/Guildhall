#include "Game/MapGenStepCellularAutomata.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Tile.hpp"

MapGenStepCellularAutomata::MapGenStepCellularAutomata( const tinyxml2::XMLElement& element ):MapGenStep(element)
{
	m_chanceToMutate = ParseXmlAttribute( element , "chanceToMutate" , m_chanceToMutate );
	m_numberOfNeighbours = ParseXmlAttribute( element , "numNeighbours" , m_numberOfNeighbours );
	m_neighbourTileType = TileDefinition::s_definitions[ element.Attribute( "neighbourTileType" ) ];
}

void MapGenStepCellularAutomata::RunStepOnce( Map& map )
{
	if ( m_setTile == nullptr )
	{
		return;
	}
	RandomNumberGenerator rng;
	int numberOfneighborsToCheckFor = m_numberOfNeighbours.GetRandomInRange( rng );
	float	chancePerTile = m_chancePerTile.GetRandomInRange( rng );
	IntVec2 tileCoords;
	std::vector<bool> hasTileChanged;
	hasTileChanged.resize( map.m_tiles.size() );
	for ( tileCoords.y = 1; tileCoords.y < map.m_height - 1; tileCoords.y++ )
	{
		for ( tileCoords.x = 1; tileCoords.x < map.m_width - 1; tileCoords.x++ )
		{
			Tile& tile = *map.GetTileAtCoords( tileCoords );
			int numMatchingNeighborTiles = GetMatchingNeighbours( map , &tile );
			int tileIndex = map.GetTileIndexForTileCoords( tileCoords );
			if ( numMatchingNeighborTiles >= numberOfneighborsToCheckFor )
			{
				if ( rng.RollPercentChance( chancePerTile ) )
				{
					hasTileChanged[ tileIndex ] = true;
				}
			}
		}
	}
	for ( int index = 0; index < hasTileChanged.size(); index++ )
	{
		if ( hasTileChanged[ index ] )
		{
			map.m_tiles[ index ].ChangeTileType( m_setTile );
		}
	}
}

int MapGenStepCellularAutomata::GetMatchingNeighbours( Map& map, Tile* tile )
{
	int numMatchingNeighbors = 0;
	IntVec2 currentTileCoords = tile->m_tileCoords;
	Tile tempTile = *map.GetNorthTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetSouthTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetEastTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetWestTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetNorthEastTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetNorthWestTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetSouthEastTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	tempTile = *map.GetSouthWestTile( currentTileCoords );
	if ( tempTile.m_definition == m_neighbourTileType )
	{
		numMatchingNeighbors++;
	}
	return  numMatchingNeighbors;
}
