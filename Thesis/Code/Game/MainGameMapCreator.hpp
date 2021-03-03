#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/Vec2.hpp"
#include "vector"
#include "string"
class Game;
class Texture;
class Camera;

enum Selections
{
	PAINT_SOLID_TILE,
	PAINT_WATER_TILE,
	PAINT_LAVA_TILE,
};

class MainGameMapCreator
{
public:
MainGameMapCreator();
~MainGameMapCreator();

void PopulateTiles();
int GetTileIndexForCoords(IntVec2 coords);
void Update( float deltaSeconds );
void Render();
void LoadData();
void SaveData();

Texture* m_groundTex = nullptr;
Texture* m_waterTileType1Main = nullptr;
Texture* m_lavaTileTypeMain = nullptr;

Camera* m_camera = nullptr;
std::vector<Tile> m_tiles;
Game* m_game = nullptr;
Vec2 m_mousePosition;
IntVec2  m_mapSize = IntVec2(80,45);
Selections m_currentSelection = PAINT_SOLID_TILE;

void SetTileType( Selections selection , IntVec2 coords );

std::string m_currentSelectedText = "Paint solid Tile";

};