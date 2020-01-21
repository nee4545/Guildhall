#pragma once
#include "Game/Tile.hpp"
#include<vector>

class Entity;
class Game;
class World;

class Map
{
private:
	friend class Game;
	friend class World;

	std::vector<Tile> m_tiles;
	std::vector<Entity*> m_enitites;
	IntVec2 tileDimensions;
	Game* m_game;
	World* m_world;

	Map(Game* game,World* world, const IntVec2& tile_dimensions);
	~Map(){};

	bool noClip=false;

private:
	void Update(float deltaSeconds);
	void Render();
	void PopulateTiles();
	void CreatePlayer();
	void ToggleNoClip();

public:
	void SetTileType(int tileX,int tileY,TileType type);
	int GetTileIndexForTileCoords(const IntVec2& tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int index);
	void PushEntityOutOfSolid(Entity *entity,const IntVec2& tileCoords);
	void HandleCollissions();
};