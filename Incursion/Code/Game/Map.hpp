#pragma once
#include "Game/Tile.hpp"
#include "Entity.hpp"
#include<vector>

class Game;
class World;

struct RayCastResult
{
public:
	bool m_didImpact=false;
	Vec2 m_impactPosition=Vec2(0.f,0.f);
	float m_impactDistance=0.f;
	float m_impactFraction=0.f;
};

class Map
{
private:
	friend class Game;
	friend class World;
	static int currentMapNumber;
	std::vector<Tile> m_tiles;
	std::vector<Vertex_PCU> tileVertices;
	EntityList m_EntityListByType[NUM_ENTITY_TYPES];
	IntVec2 tileDimensions;
	Game* m_game;
	World* m_world;
	RandomNumberGenerator rng;

	Map(Game* game,World* world, const IntVec2& tile_dimensions);
	~Map(){};

	bool noClip=false;
	Vec2 m_exitPosition = Vec2((tileDimensions.x-2)+0.5f,(tileDimensions.y-2)+0.5f);

	TileType defaultTile = TILE_TYPE_GRASS;
	TileType startTile = TILE_TYPE_START;
	TileType endTile = TILe_TYPE_EXIT;
	TileType wormTile = TILE_TYPE_STONE;
	TileType edgeTile = TILE_TYPE_EDGE ;

private:
	void Update(float deltaSeconds);
	void Render();
	void PopulateTiles();
	void ToggleNoClip();

public:
	void SetTileType(int tileX,int tileY,TileType type);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int index);
	void PushEntityOutOfSolid(Entity *entity,const IntVec2& tileCoords);
	void HandleBlockCollissions();
	void HandleDiscCollissions();
	void HandleBulletEntityCollissions();
	void HandleAirStrikeCollissions();
	void SpawnNewEntity(EntityType entityType, EntityFaction faction,const Vec2& spawnPosition );
	void AddEntityToMap(Entity* entity,EntityType entityType);
	RayCastResult Raycast(Vec2 Start,Vec2 fwdDir,float maxDistance);
	bool IsTileSolid(IntVec2 &tileCoords);
	bool IsPointInSolid(Vec2 point);
	bool HasLineOfSight(Vec2 destination,Vec2 source, float maxDistance);
	Vec2 GetPlayerPosition();
	void SpawnBullet(Vec2 position,EntityType entityType, float orientationDegrees);
	void DeleteGarbageEntities();
	void DeflectBullets();
	bool IsPlayerAlive();
	void SpawnEntites(int num_NPCTurrets, int num_NPCTanks,int num_Boulders);
	void ManagePlayerTankSpeed();
	TileType GetTileTypeForPoint(Vec2 point);
	void DetectWin();
	void MapFillUsingWorms(TileType type);
	bool IsMapValidityUsingFloodFill();
	void CreateExplosions(float explosionTime, float explosionSize, Vec2 position);
	void CreateAirPlane(Vec2 planePosition, Vec2 airStrikePosition);
	void CreateAirStrike(Vec2 position);
	
};

