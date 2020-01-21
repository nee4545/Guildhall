#pragma once
#include "Engine/Math/IntVec2.hpp"

class TileDefinition;

class Tile
{
public:
	explicit Tile( int tileX,int tileY,TileDefinition* definition );
	explicit Tile( IntVec2 tileCoords,TileDefinition* definition );
	~Tile() {};
	IntVec2 m_tileCoords;
	TileDefinition* m_definition= nullptr;

	void ChangeTileType(TileDefinition* newType);

};