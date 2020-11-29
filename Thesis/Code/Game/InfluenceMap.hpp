#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

class Game;

struct InfluenceMapNode
{
	IntVec2 coords;
	int influenceValue = 0;
};

class InfluenceMap
{
public:
	InfluenceMap(Game* game,IntVec2 startPos, IntVec2 dimesions, int startValue = 1, int fallOff = 1);
	~InfluenceMap();

	IntVec2 m_startPosition;
	IntVec2 m_dimesions;
	int m_startValue = 1;
	int m_fallOffPerTile = 1;
	std::vector<InfluenceMapNode> m_nodes;
	Game* m_game = nullptr;

	void Create();
	void ClearInfluence();
	IntVec2 GetMaxValueCoords();
	void ClearInfluenceAtNode( IntVec2 coords );
	void Correct();

};