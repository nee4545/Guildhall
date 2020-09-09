#include "Game/Tile.hpp"

Tile::Tile( int tileX , int tileY , TileType tileType /*= TILE_TYPE_SAND */ )
{
	m_tileCoords.x = tileX;
	m_tileCoords.y = tileY;
	m_type = tileType;
}
