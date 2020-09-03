#pragma once

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/AABB3.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Game/Map.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Math/Vec2.hpp"

#include <vector>
#include <map>

struct Vertex_PCU;
class MapMaterial;
class MapRegionType;
class GpuMesh;
class Portal;


class Tile
{
	friend class TileMap;

public:
	IntVec2 m_tileCoords;
	//AABB3 m_bounds;
	bool m_isSolid = false;
	bool IsSolid();

	const MapRegionType* m_region = nullptr;

};


class TileMap :public Map
{
public:
	GPUMesh* m_debugMesh = nullptr;
	TileMap( char const* mapName , IntVec2 dimensions );
	TileMap( char const* mapName , tinyxml2::XMLElement* mapXmlElement );


	virtual ~TileMap();

	void PopulateTiles();
	void ParseLegend(tinyxml2::XMLElement* legendElement, std::map<char , std::string>& legendMap );
	void ParseMapRows( tinyxml2::XMLElement* rowElement, std::map<char , std::string>& legendMap );
	void Update( float deltaSeconds ) override;
	void UpdateMeshes() override;
	void Render() override;
	void AddVertsForTile( std::vector<Vertex_PCU>& destination , int tileIndex );
	void AddVertsForSolidTile( std::vector<Vertex_PCU>& destination , int tileIndex );
	void AddVertsForNonSolidTile( std::vector<Vertex_PCU>& destination , int tileIndex );
	AABB3 GetTilebounds( int tileIndex );

	IntVec2 GetTileCoordsForIndex( int index );
	int GetIndexForTileCoords( IntVec2 tileCoords );
	Tile* GetTileAtCoords( IntVec2 tileCoords );
	bool isTileSolid( IntVec2 tileCoords );
	void ResolveEntityVsTileCollisions( Entity* entity  );
	void CheckForPortalCollissions();

	RayCastResult RayCastToEntities( Vec2 start , Vec2 direction , float distance );
	RayCastResult RayCastToTiles( Vec2 start , Vec2 direction , float distace );

	void DebugRenderRaycasts( Vec2 start , Vec2 direction , float distance );

	
private:
	IntVec2 m_dimenstions;
	std::vector<Tile> m_tiles;
	GPUMesh* m_worldMesh=nullptr;
	std::vector<Portal*> m_portals;
	

};