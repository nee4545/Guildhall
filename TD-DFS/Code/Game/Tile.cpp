#include "Game/Tile.hpp"
#include "Game/GameCommon.hpp"

Tile::Tile( int tileX, int tileY, TileType tileType )
{
	m_tileCoords.x=tileX;
	m_tileCoords.y=tileY;
	m_type=tileType;
}

void Tile::Render()
{
	
}
