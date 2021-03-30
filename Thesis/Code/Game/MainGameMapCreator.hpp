#pragma once
#include "Game/Tile.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Game/EntityCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/AABB2.hpp"
#include "vector"
#include "string"

class Game;
class Texture;
class Camera;
class InfluenceMap;
class Entity;
class MonsterAI;
class PotentialFieldPathFinder;
class Player;
class OccAI;
class Bomb;

enum TileSelections
{
	PAINT_SAND_TILE,
	PAINT_SOLID_TILE,
	PAINT_WATER_TILE,
	PAINT_LAVA_TILE,
	PAINT_INFLUENCE_MAP,
	PAINT_MONSTER_BASE,
	PAINT_AI_TYPE_1,
	PAINT_AI_TYPE_2,
	PAINT_AI_TYPE_3,
	PAINT_OCC_AI,
	DEBUG_POTENTIAL_FINDER_COST,
	DEBUG_POTENTIAL_FNDER_INTEGRATION_COST,
	DEBUG_FLOW_FIELDS,
	SET_PLAYER_POSITION,
};

enum TileSubSelections
{
	PAINT_DEFAULT,
	PAINT_UP,
	PAINT_DOWN,
	PAINT_LEFT,
	PAINT_RIGHT,
	PAINT_TOPLEFT,
	PAINT_TOPRIGHT,
	PAINT_BOTRIGHT,
	PAINT_BOTLEFT,

};

class MainGameMapCreator
{
public:
MainGameMapCreator(Game* game);
~MainGameMapCreator();

void PopulateTiles();
int GetTileIndexForCoords(IntVec2 coords);
void Update( float deltaSeconds );
void UpdateCamera();
void UpdateTileState();
void SpawnBomb( Vec2 position );
void HandleBlockCollissions( Entity* entity );
bool IsTileSolid(IntVec2 coords);
void Render();
void LoadData();
void SaveData();


//Camera Controls
int m_zoomFactor = 1;
Vec2 m_camerapos;


PotentialFieldPathFinder* m_finder = nullptr;

Texture* m_sandTex = nullptr;

Texture* m_groundTex = nullptr;
Texture* m_groundUpTex = nullptr;
Texture* m_groundBotTex = nullptr;
Texture* m_groundLeftTex = nullptr;
Texture* m_groundRightTex = nullptr;
Texture* m_groundTopRightTex = nullptr;
Texture* m_groundTopLeftTex = nullptr;
Texture* m_groundBottomRightTex = nullptr;
Texture* m_groundBottomLeftTex = nullptr;

Texture* m_waterTileType1Main = nullptr;
Texture* m_waterUpTex = nullptr;
Texture* m_waterBotTex = nullptr;
Texture* m_waterLeftTex = nullptr;
Texture* m_waterRightTex = nullptr;
Texture* m_waterTopRightTex = nullptr;
Texture* m_waterTopLeftTex = nullptr;
Texture* m_waterBottomRightTex = nullptr;
Texture* m_waterBottomLeftTex = nullptr;

Texture* m_lavaTileTypeMain = nullptr;
Texture* m_lavaTileTypeMain1 = nullptr;
Texture* m_lavaUpTex = nullptr;
Texture* m_lavaBotTex = nullptr;
Texture* m_lavaLeftTex = nullptr;
Texture* m_lavaRightTex = nullptr;
Texture* m_lavaTopRightTex = nullptr;
Texture* m_lavaTopLeftTex = nullptr;
Texture* m_lavaBottomRightTex = nullptr;
Texture* m_lavaBottomLeftTex = nullptr;

Texture* m_monsterBaseTex = nullptr;
Texture* m_sandTileTex = nullptr;

Camera* m_camera = nullptr;
Camera* m_debugCamera = nullptr;
Game* m_game = nullptr;
Vec2 m_mousePosition;
bool m_debugPFFcost = false;
int m_currentFinderTileIndex = 0;
IntVec2  m_mapSize = IntVec2(80,45);
TileSelections m_currentSelection = PAINT_SOLID_TILE;
TileSubSelections m_currentSubSelection = PAINT_DEFAULT;

void SetTileType( TileSelections selection , IntVec2 coords );

std::string m_currentSelectedText = "Paint solid Tile";

std::vector<Tile> m_tiles;
std::vector<InfluenceMap*> m_infMaps;

IntVec2 m_monsterBasedimensions;

bool m_updateAI = false;

int m_infMapCurrentStartValue = 5.f;
IntVec2 m_infMapDimensions = IntVec2( 5 , 5 );
int m_infMapCurrentFalloff = 1.f;

std::vector<AABB2*> m_monsterBases;

std::vector<MonsterAI*> m_ais;
std::vector<OccAI*> m_occAis;
std::vector<Bomb*> m_bombs;

Player* m_player = nullptr;

OCCAI_Type m_currentType = OCC_AI_SINGLE_MAP;

bool m_gamePaused = true;

std::vector<Vertex_PCU> m_mainTiles;
std::vector<Vertex_PCU> m_solidTiles;

std::vector<Vertex_PCU> m_waterTiles;
std::vector<Vertex_PCU> m_waterBotTiles;
std::vector<Vertex_PCU> m_waterTopTiles;
std::vector<Vertex_PCU> m_waterLeftTiles;
std::vector<Vertex_PCU> m_waterRightTiles;
std::vector<Vertex_PCU> m_waterTopRightTiles;
std::vector<Vertex_PCU> m_waterBottomRightTiles;
std::vector<Vertex_PCU> m_waterTopLeftTiles;
std::vector<Vertex_PCU> m_waterBottomLeftTiles;


std::vector<Vertex_PCU> m_lavaTiles;
std::vector<Vertex_PCU> m_lavaBotTiles;
std::vector<Vertex_PCU> m_lavaTopTiles;
std::vector<Vertex_PCU> m_lavaLeftTiles;
std::vector<Vertex_PCU> m_lavaRightTiles;
std::vector<Vertex_PCU> m_lavaTopRightTiles;
std::vector<Vertex_PCU> m_lavaBottomRightTiles;
std::vector<Vertex_PCU> m_lavaTopLeftTiles;
std::vector<Vertex_PCU> m_lavaBottomLeftTiles;

bool debug_AI_InfluenceMaps = false;
bool debug_Map_InfluenceMaps = false;


AABB2 m_fps_box;

};