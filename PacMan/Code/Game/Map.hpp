#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include<vector>
#include<map>

class Entity;
class Game;
class Player;
class AIType1;
class AIType2;
class AIType3;
class Collectable;
class SpecialCollectable;

class Map
{
private:
	friend class Game;
	friend class World;
	RandomNumberGenerator rng = RandomNumberGenerator();
	int m_collectableCounter = 0;
	int m_SplCollectableCounter = 0;

	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_enitites;
	std::vector<Vertex_PCU> m_tileVertices;
	std::vector<Collectable*> m_collectables;
	
	IntVec2 m_tileDimensions;
	Game* m_game = nullptr;
	Player* m_player = nullptr;
	AIType1* m_aiOne = nullptr;
	AIType2* m_aiTwo = nullptr;
	AIType3* m_aiThree = nullptr;
	SpecialCollectable* m_specialCollectables[ 5 ] = { nullptr };

	explicit Map(Game* game,const IntVec2& tile_dimensions);
	~Map(){};

	bool noClip=false;

private:
	void Update(float deltaSeconds);
	void Render();
	void RenderRespawnScreen();
	void PopulateTiles();
	void PoplulateCollectables();
	void CreatePlayer();
	void ToggleNoClip();
	

public:
	std::map<int , int[ 900 ]> m_distanceFields;

	void SetTileType(int tileX,int tileY,TileType type);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int index);
	void HandleCollissions();
	void HandleCollectableCollissions();
	void HandleEntityCollissions();
	bool CanEntityMoveOnTile( IntVec2 tileCoords );
	void SetMapMetaData();
	void SetDistanceField( int goalTileIndex );
	bool IsTileSolid( IntVec2 tileCoords );
	void DisplayDistanceFieldInfo();
	Vec2 GetPlayerPosition();
	void StartUp();
	void DetectWinOrLoss();
	void DeleteGarbageEntites();
	bool IsPlayerAlive();
	void RespawnPlayer();
};