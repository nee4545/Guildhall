#pragma once
#include "Engine/Math/IntVec2.hpp"

enum TileType
{
	TILE_TYPE_GRASS=0,
	TILE_TYPE_STONE,
	TILE_TYPE_MUD,
	TILe_TYPE_EXIT,
	TILE_TYPE_START,
	TILE_TYPE_EDGE,
	NUM_TILE_TYPES

};

class Tile
{
public:

	Tile();
	~Tile(){};
	Tile(int tileX,int tileY, TileType tileType=TILE_TYPE_GRASS);
	IntVec2 m_tileCoords;
	TileType m_type;

	//meta data for flood fill
	bool hasBeenSeen = false;

	void Render();
};