#include "Game/MapGenStepMutate.hpp"
#include "Game/Map.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


MapGenStepMutate::MapGenStepMutate( const tinyxml2::XMLElement& element ) : MapGenStep(element)
{
	m_chanceToMutate = ParseXmlAttribute( element , "chanceToMutate" , m_chanceToMutate );
}

void MapGenStepMutate::RunStepOnce( Map& map )
{
	RandomNumberGenerator rng = RandomNumberGenerator();
	float chancePerTile = m_chancePerTile.GetRandomInRange( rng );

	IntVec2 tileCoords;

	for ( tileCoords.y = 0; tileCoords.y < map.m_height; tileCoords.y++ )
	{
		for ( tileCoords.x = 0; tileCoords.x < map.m_width; tileCoords.x++ )
		{
			Tile* tile = map.GetTileAtCoords( tileCoords );
			if ( tile->m_definition == m_ifTile )
			{
				float chance = rng.RollPercentChance( chancePerTile );
				if (  chance )
				{
					tile->ChangeTileType( m_setTile );
				}
			}
		}
	}
}
