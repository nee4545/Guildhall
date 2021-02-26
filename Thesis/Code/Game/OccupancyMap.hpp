#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <map>
#include <string>

class Game;

struct OccupancyMapNode
{
	IntVec2 coords;
	float value =0.f;
	bool hasInfluence = false;
};

class OccupancyMap
{
public:
	OccupancyMap(Game* game,IntVec2 anchorPoint, IntVec2 dimensions, float maxValue);
	OccupancyMap( Game* game , IntVec2 anchorPoint , IntVec2 dimensions );
	~OccupancyMap();


public:
	std::vector<OccupancyMapNode> m_nodes;
	std::vector<int> m_nodesToIndicesPropogate;
	std::map<std::string , int> m_dict;
	IntVec2 m_dimensions;
	IntVec2 m_anchorPoint;
	float m_currentMinValue = 0.f;
	float m_maxValue;
	Game* m_game = nullptr;
	
	
	void Create();
	void CreateForOccMapGame();
	void PropgateInfluence();
	IntVec2 GetMaxValueCoords();
	std::vector<IntVec2> GetAllMaxValueCoords();
	float GetMaxValue();
	int  GetIndexForCoords( IntVec2 coords );
	void SetValue( IntVec2 coords, float value );
	void DebugRender();

	
	
};