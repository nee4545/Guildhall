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
	Rgba8 grassColor=Rgba8(0,80,0,255);
	Rgba8 stoneColor=Rgba8(100,100,100,255);

	render->BindTexture(nullptr);
	if(m_type==TILE_TYPE_GRASS )
	{ 
	     render->DrawAABB2D(AABB2((float)m_tileCoords.x,(float)m_tileCoords.y,(float)m_tileCoords.x+1,(float)m_tileCoords.y+1),grassColor);
	}
	else
	{
		render->DrawAABB2D(AABB2((float)m_tileCoords.x,(float)m_tileCoords.y,(float)m_tileCoords.x+1,(float)m_tileCoords.y+1),stoneColor);
	}
}
