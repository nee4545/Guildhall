#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

class Tile;

struct InfluenceMapNode
{
	IntVec2 coords;
	int influenceValue = 0;
};

class InfluenceMap
{
public:
	InfluenceMap(IntVec2 startPos, bool isPositive, IntVec2 dimensions, int fallOff = 1, int startValue =0);
	~InfluenceMap();

	IntVec2 m_startPosition;
	int m_fallOff = 1;
	int m_startValue = 0;
	IntVec2 m_dimensions;
	std::vector<InfluenceMapNode> m_nodes;

	void Create();
	void ClearInfluence();
	void SetInfluenceToTiles( std::vector<Tile>& tiles );
};