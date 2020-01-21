#pragma once
#include "Engine/Math/IntVec2.hpp"

enum TileType
{
	TILE_TYPE_GRASS=0,
	TILE_TYPE_STONE,
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

	void Update(float deltaSeconds);
	void Render();
};