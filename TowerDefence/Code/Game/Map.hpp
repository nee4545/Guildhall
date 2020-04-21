#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Game/GameManager.hpp"
#include<vector>


class Entity;
class Hero;
class Wall;
class Spike;
class Dino;
class Game;
class HUD;
class AIController;
class Texture;
class SpriteAnimDefTex;

enum SelectedUnit
{
	UNIT_NONE = 0 ,
	UNIT_HERO ,
	UNIT_SPIKE ,
	UNIT_BOX ,
	UNIT_DINO
};

enum Lanes
{
	LANE_1,
	LANE_2,
	LANE_3,
	LANE_4
};

class Map
{
public:
	friend class Game;


	std::vector<Tile> m_tiles;

	IntVec2 tileDimensions;
	std::vector<Vertex_PCU> m_tileVertices;
	Game* m_game;
	HUD* m_HUD;
	
	AABB2 m_lane0 = AABB2( 3.5f , 2.f , 23.5 , 7.f );
	AABB2 m_lane1 = AABB2( 3.5f , 7.f , 23.5f , 12.f );
	AABB2 m_lane2 = AABB2( 3.5f , 12.f , 23.5f , 17.f );
	AABB2 m_lane3 = AABB2( 3.5f , 17.f , 23.5f , 22.f );

	SelectedUnit m_selcetedUnit = UNIT_NONE;

	std::vector<Hero*> m_Playerheros;
	std::vector<Hero*> m_Enemyheros;
	std::vector<Wall*> m_walls;
	std::vector<Spike*> m_spikes;
	std::vector<Dino*> m_dinos;

	AIController* m_enemyController = nullptr;

	
	Map(Game* game, const IntVec2& tile_dimensions);
	~Map(){};

	bool noClip=false;
	GameManager m_manager;

private:
	void Update(float deltaSeconds);
	void Render();
	void PopulateTiles();
	void RenderBackRounds();
	void CreatePlayer();
	void ToggleNoClip();
	void LoadTextures();

public:
	void SetTileType(int tileX,int tileY,TileType type);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int index);
	void PushEntityOutOfSolid(Entity *entity,const IntVec2& tileCoords);
	void HandleCollissions();
	void DisplayLanes();
	void UpdatedSelectedUnits();
	void SpawnSelectedUnit();
	void SpawnEnemyUnit(Lanes lane,SelectedUnit unit);
	void SpawnEnemySpike( Lanes lane , float xPosition );
	void HandleHeroPositions();
	void HandleDinoPositions();
	void DamagePlayer( float damageAmt );
	void DamageEnemy( float damageAmt );
	void HandleDeletion();
	void HandelHerovsHeroCollissions();
	void HandelHerovsSpikeCollissions();
	void HandelHerovsDinoCollissions();
	int GetNumberOfHerosInLane( int laneNumber );
	int GetNumberOfSpikesInLane( int laneNumber );
	void IncreasePlayerCoins( int increament );
	void IncreaseEnemyCoins( int increament );


public:
	std::vector<Texture*> m_playerRunning;
	std::vector<Texture*> m_playerAttacking;
	std::vector<Texture*> m_enemyRunning;
	std::vector<Texture*> m_enemyAttacking;

	SpriteAnimDefTex* playerRunning = nullptr;
	SpriteAnimDefTex* playerAttacking = nullptr;

	SpriteAnimDefTex* enemyRunning = nullptr;
	SpriteAnimDefTex* enemyAttacking = nullptr;

};