#pragma once
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include<vector>

class Entity;
class Hero;
class Wall;
class Spike;
class Dino;
class Game;
class HUD;

enum SelectedUnit
{
	UNIT_NONE = 0 ,
	UNIT_HERO ,
	UNIT_SPIKE ,
	UNIT_BOX ,
	UNIT_DINO
};

class Map
{
private:
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

	std::vector<Hero*> m_heros;
	std::vector<Wall*> m_walls;
	std::vector<Spike*> m_spikes;
	std::vector<Dino*> m_dinos;


	Map(Game* game, const IntVec2& tile_dimensions);
	~Map(){};

	bool noClip=false;

private:
	void Update(float deltaSeconds);
	void Render();
	void PopulateTiles();
	void RenderBackRounds();
	void CreatePlayer();
	void ToggleNoClip();

public:
	void SetTileType(int tileX,int tileY,TileType type);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int index);
	void PushEntityOutOfSolid(Entity *entity,const IntVec2& tileCoords);
	void HandleCollissions();
	void DisplayLanes();
	void UpdatedSelectedUnits();
	void SpawnSelectedUnit();

};