#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

struct InfluenceMapNode
{
	IntVec2 coords;
	int influenceValue = 0;
};

class InfluenceMap
{
public:
	InfluenceMap(IntVec2 startPos, bool isPositive, int size);
	~InfluenceMap();

	IntVec2 m_startPosition;
	bool m_isPositive = false;
	int m_size = 1;
	std::vector<InfluenceMapNode> m_nodes;

	void Create();
	void ClearInfluence();

};