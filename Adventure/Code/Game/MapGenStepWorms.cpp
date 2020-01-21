#include "Game/MapGenStepWorms.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

MapGenStepWorms::MapGenStepWorms( const tinyxml2::XMLElement& element ):MapGenStep(element)
{
	m_wormLength = ParseXmlAttribute( element , "wormLength" , m_wormLength );
	m_numWorms = ParseXmlAttribute( element , "numWorms" , m_numWorms );
}

void MapGenStepWorms::RunStepOnce( Map& map )
{
	RandomNumberGenerator rng;
	int numberOfWorms = m_numWorms.GetRandomInRange( rng );
	for ( int currentWormNum = 0; currentWormNum < numberOfWorms; currentWormNum++ )
	{
		IntVec2 tileCoords;
		tileCoords.x = rng.RollRandomIntInRange( 1 , map.m_width - 2 );
		tileCoords.y = rng.RollRandomIntInRange( 1 , map.m_height - 2 );
		int wormLength = m_wormLength.GetRandomInRange( rng );
		//int wormCurrentLength = 0;
		int tileIndex = map.GetTileIndexForTileCoords( tileCoords );
		for ( int wormCurrentLength = 0; wormCurrentLength < wormLength; wormCurrentLength++ )
		{
			IntVec2 nextTile = tileCoords;
			int nextTileMovement = rng.RollRandomIntInRange( 0 , 1 );
			int nextTileVariation = rng.RollRandomIntInRange( -1 , 1 );
			if ( nextTileMovement == 0 )
			{
				tileCoords.x += nextTileVariation;
			}
			else
			{
				tileCoords.y += nextTileVariation;
			}
			tileIndex = ( tileCoords.y * map.m_width ) + tileCoords.x;
			int currentTileIndex = map.GetTileIndexForTileCoords( tileCoords );
			if ( currentTileIndex > 0 && currentTileIndex < ( map.m_height * map.m_width ) )
			{
				map.m_tiles[ currentTileIndex ].ChangeTileType( m_setTile );
			}
		}
	}

}
