#pragma once
#include "Engine/Math/IntVec2.hpp"

enum TileType
{
	TILE_TYPE_WOOD=0,
	TILE_TYPE_STONE,
	TILE_TYPE_ROCK,
	NUM_TILE_TYPES

};

class Tile
{
public:

	Tile();
	~Tile(){};
	Tile(int tileX,int tileY, TileType tileType=TILE_TYPE_WOOD);
	IntVec2 m_tileCoords;
	TileType m_type;
	bool m_hasBeenSeen = false;

	void Update(float deltaSeconds);
	void Render();
};