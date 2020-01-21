#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"

Tile::Tile( int tileX, int tileY, TileDefinition* definition )
{
	m_tileCoords = IntVec2(tileX,tileY);
	m_definition = definition;
}

Tile::Tile( IntVec2 tileCoords, TileDefinition* definition )
{
	m_tileCoords = tileCoords;
	m_definition = definition;
}

void Tile::ChangeTileType( TileDefinition* newType )
{
	m_definition = newType;
}
