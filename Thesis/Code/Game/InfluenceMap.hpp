#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>
#include <map>
#include <string>

class Game;
class OccupancyMap;

enum MapType
{
	POSITVE,
	NEGATIVE,
	MIXED,
};


struct InfluenceMapNode
{
	IntVec2 coords;
	float influenceValue = 0;
};

class InfluenceMap
{
public:
	InfluenceMap(Game* game,IntVec2 startPos, IntVec2 dimesions, float startValue = 1.f, float fallOff = 1.f);
	InfluenceMap( Game* game );
	~InfluenceMap();

	IntVec2 m_startPosition;
	IntVec2 m_dimesions;
	std::map<std::string , int> m_dict;
	float m_startValue = 1;
	float m_fallOffPerTile = 1.f;
	std::vector<InfluenceMapNode> m_nodes;
	Game* m_game = nullptr;

	void Create();
	void ClearInfluence();
	IntVec2 GetMaxValueCoords();
	int GetIndexForCoords(IntVec2 coords);

	void ClearInfluenceAtNode( IntVec2 coords );
	IntVec2 GetCoordsWithValue( float value );
	std::vector<IntVec2> GetAllCoordsWithValue( float value );
	void Correct();
	void AddFromMap( InfluenceMap* mapToAdd );
	void AddFromOccupancyMap( OccupancyMap* mapToAdd );
	void SubractFromMap( InfluenceMap* mapToSubract );

};