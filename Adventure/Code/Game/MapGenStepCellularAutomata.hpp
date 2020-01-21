#pragma once
#include "Game/MapGenStep.hpp"
class Tile;
class Map;

class MapGenStepCellularAutomata : public MapGenStep
{
public:
	MapGenStepCellularAutomata( const tinyxml2::XMLElement& element );
	~MapGenStepCellularAutomata() {};

	virtual void RunStepOnce( Map& map ) override;
	int GetMatchingNeighbours( Map& map, Tile* tile );
	

private:

	FloatRange m_chanceToMutate = FloatRange( 1.f );
	IntRange m_numberOfNeighbours = IntRange( 1 );
	TileDefinition* m_neighbourTileType = nullptr;

};
