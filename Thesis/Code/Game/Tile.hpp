#pragma once
#include "Engine/Math/IntVec2.hpp"

class Texture;

enum TileType
{
	TILE_TYPE_SAND = 0 ,
	TILE_TYPE_STONE ,
	TILE_TYPE_WATER ,
	TILE_TYPE_LAVA,
	NUM_TILE_TYPES

};

enum TileSubType
{
	TILE_TYPE_NONE,
	TILE_TYPE_UP,
	TILE_TYPE_DOWN,
	TILE_TYPE_LEFT,
	TILE_TYPE_RIGHT,
	TILE_TYPE_TOP_LEFT,
	TILE_TYPE_BOT_LEFT,
	TILE_TYPE_BOT_RIGHT,
	TILE_TYPE_TOP_RIGHT,
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
	TileSubType m_subType = TILE_TYPE_NONE;
	TileTextureType m_textureType = TILE_INVALID;
	bool m_isSolid = false;
	bool inPath = false;
	int m_influenceValue = 0;
	bool m_doesHaveDirection = false;
	bool m_isDirectionPositive = false;
	float m_direction = 0.f;
	float m_directionIntensity = 0.f;

};