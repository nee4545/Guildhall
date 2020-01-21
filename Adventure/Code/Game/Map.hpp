#pragma once
#include "Game/Tile.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include<vector>
#include <string>

class MapDefinition;
class Texture;
class Game;
class Actor;
class NpcUnit;
class Entity;

class Map
{
public:

	explicit Map(Game* game, std::string mapName, MapDefinition* mapDefinition );
	void Update(float deltaSeconds);
	void Render(); 
	void PopulateTiles();
	int GetTileIndexForTileCoords( const IntVec2& tileCoords ) const;
	void SetTileType( int tileX, int tileY, TileDefinition* newDef );
	Tile* GetTileAtCoords( IntVec2 tileCoords );
	IntVec2 GetTileInfoForMousePosition(Vec2 mousePositionInCameraView);
	std::string GetTileInfoForTileCoords( IntVec2 tileCoords );
	void HandleBlockCollissions();
	bool CanEntityMoveOnTile( Entity* entity , int tileIndex );
	bool IsMousePointingActor( Vec2 mousePositionInCameraView );
	bool IsMousePointingNpcUnits( Vec2 mousePositionInCameraView );
	Strings GetPlayerDetails();
	Strings GetNpcDetails( Vec2 mousePositionInCameraView );
	Texture* m_tiletexture;
	Tile* GetNorthTile( IntVec2 currentTileCoords );
	Tile* GetSouthTile( IntVec2 currentTileCoords );
	Tile* GetEastTile( IntVec2 currentTileCoords );
	Tile* GetWestTile( IntVec2 currentTileCoords );
	Tile* GetNorthEastTile( IntVec2 currentTileCoords );
	Tile* GetNorthWestTile( IntVec2 currentTileCoords );
	Tile* GetSouthEastTile( IntVec2 currentTileCoords );
	Tile* GetSouthWestTile( IntVec2 currentTileCoords );

	int m_width;
	int m_height;
	std::vector<Tile> m_tiles;
	Actor* m_player = nullptr;

private:

	std::string m_name;
	std::vector<Vertex_PCU> m_tileVertices;

	MapDefinition* m_definition = nullptr;
	Game* m_game = nullptr;
	std::vector<NpcUnit*> m_NpcUnits;
};