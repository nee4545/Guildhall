#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>
#include <map>

class Game;

struct CostField
{
	int tileIndex =0;
	int cost = 0;
};

struct FlowField
{
	int tileIndex = 0;
	Vec2 flowDirection;
};

struct IntegrationField
{
	int tileIndex = 0;
	int cost = 0;
};

class PotentialFieldPathFinder
{
public:
	PotentialFieldPathFinder( Game* game , std::vector<Tile> tiles );
	~PotentialFieldPathFinder();

	std::vector<Tile> m_tiles;
	IntVec2 m_mapSize = IntVec2( 80 , 45 );
	Game* m_game = nullptr;
	std::map<int , std::vector<CostField>> m_costFields;
	std::map<int , std::vector<IntegrationField>> m_intergationFields;
	std::map<int , std::vector<FlowField>> m_flowFields;

	void GenerateCostFields();
	void GenerateIntegrationFields();
	void GenerateFlowFields();
	void GenerateIntegrationFieldForTile( int tileIndex , std::vector<CostField>& costFields );
	void GenerateFlowFieldForTile( int tildeIndex , std::vector<IntegrationField>& integrationFields );
	void DebugRenderCostFields(int index);
	void DebugRenderIntegrationFields( int index );
	void DebugRenderFlowFields( int index );
	int GetTileIndexForCoords( IntVec2 coords );
};