#pragma once
#include "Engine/Math/IntVec2.hpp"

class Texture;

enum TileType
{
	TILE_TYPE_SAND = 0 ,
	TILE_TYPE_STONE ,
	TILE_TYPE_WATER ,
	NUM_TILE_TYPES

};

enum TileTextureType
{
	TILE_INVALID,
	TILE_SAND_MAIN,
	TILE_SAND_LEFT,
	TILE_SAND_RIGHT,
	TILE_SAND_BOT,
	TILE_SAND_TOP,
	TILE_SAND_TOPRIGHT,
	TILE_SAND_TOPLEFT,
	TILE_SAND_BOTTOMLEFT,
	TILE_SAND_BOTTOMRIGHT,
	TILE_BLOCK,
};

class Tile
{
public:

	Tile();
	~Tile() {};
	Tile( int tileX , int tileY , TileType tileType = TILE_TYPE_SAND );
	IntVec2 m_tileCoords;
	TileType m_type;
	TileTextureType m_textureType = TILE_INVALID;
	bool m_isSolid = false;
};