#pragma once
#include <vector>
#include "GameCommon.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/OccupancyMapCollectable.hpp"


class SpriteSheet;
class Entity;
class SpriteAnimDefTex;
class Camera;
class Player;
class SupportPlayer;
class SpriteAnimDefinition;
class MonsterAI;
class RandomNumberGenerator;
class StartScreen;
class Bomb;
class Turret;
class GreenBeret;
class MapCreator;
class PotentialFieldCreator;
class PotentialField;
class Timer;
class SymmetricPotentialField;
class OccAI;
class BitmapFont;
class MainGameMapCreator;
class InfluenceMap;

enum GameMode
{
	GAME,
	PATHFINDER,
	MAPCREATOR,
	POTENTIALFIELD_CREATOR,
	OCCUPANCY_MAP_GAME,
	INF_AND_PF_MAP_GAME,
};

struct PathFindingHelper
{
public:
	IntVec2 coords;
	float gCost;
	float hCost;
	bool isConsidered = false;
	PathFindingHelper* parent = nullptr;
	bool isClosed = false;
};

struct RayCastResult
{
	bool didImpact = false;
	Vec2 startHitPoint;
	Vec2 endHitPoint;
	Vec2 impactNormal;
};

class Game
{

public:

	Game();
	~Game();

	void Update( float deltaseconds );
	void Render();
	void LoadPlayerTextures();
	void LoadSupportPlayerTextures();
	void LoadAIAnimations();
	void LoadBombAnimations();

	void LoadPlayerHudAnimations();

	void UpdatePathFinderMode();
	void RefreshPathFinderMode();
	void RenderPathFinderMode();

	void UpdateCamera();
	void ToggleCameraUpdate();
	void UpdateMousePosition();
	void TogglePlayers();
	void ToggleGameModes();
	void HandleOccGameObjectGarbageCollection();

	void PopulateTiles();
	void HandleBlockCollissions(Entity* entity);
	bool IsTileSolid( IntVec2 tileCoords, bool isInOccGame=false );
	
	void GetPathUsingAStarIgnoreDiagonalMovesOneStep(Vec2 startPos, Vec2 endPos,std::vector<int>& path, bool ignoreDiagonalMoves = false, bool considerInfluenceMaps = false);
	void GetPathUsingAstarWithDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path );
	void GetPathUsingAStarIgnoreDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path , bool considerInfuenceMap = false , bool ignoreDiagonalMoves = false );
	void GetPathInGame( Vec2 startPos , Vec2 endPos , std::vector<int>& path , bool considerInfluenceMap = false , bool ignoreDiagonalMoves = false );
	void GetPathInOccupancyGame(Vec2 startPos,Vec2 endPos,std::vector<int>& path, bool considerInfueneMap = false, bool ignoreDiagonalMoves = false);
	void GetPath( Vec2 startPos , Vec2 endPos , std::vector<Tile> tiles , std::vector<int>& path , bool considerInfulenceMap = false , bool ignoreDiagonalMoves = false , InfluenceMap* infMap = nullptr );
	float GetManhattanDistance( IntVec2 currentPos , IntVec2 finalPos );

	void CreateInfluenceMap( IntVec2 startCoords , bool isPositive , int initialValue );

	void GetAllTilesWithSameFCost( std::vector<PathFindingHelper>& list , std::vector<PathFindingHelper>& outList );
	bool IsPathFindingHelpInList( PathFindingHelper toFind , std::vector<PathFindingHelper>& list );

	RayCastResult RayCast( Vec2 start , Vec2 direction , float distance );
	RayCastResult RayCastInOccGame( Vec2 start , Vec2 direction , float distance );
	RayCastResult RayCastFinal( Vec2 start , Vec2 direction , float distanc , std::vector<Tile> tilesToConsider );
	

	void ToggleDevConsole();

	int GetTileIndexForTileCoords( const IntVec2& tileCoords, bool usingMainMap = false );
	int GetTileIndexForOccGame( const IntVec2& tileCoords );
	int GetTileIndexForTileCoordsForMainMap( const IntVec2& tileCoords );
	void MapFillUsingWorms( TileTextureType type, int minWorkLength = 4, int maxWorkLength =12 , int maxWorms = 90);

	void LoadDataFromXml();
	void LoadPotentialFieldFromXML();
	void UpdateTileState();

	void LoadOccpancyGameDataFromXml();
	void SaveOccupancyGameData();
	void RenderOccUI();
	Texture* GetGetTextureForNumber( int number );
	void CheckForWinOrLoss();

	MainGameMapCreator* m_mainMapCreator = nullptr;

	GameMode m_currentMode = PATHFINDER;
	StartScreen* m_startScreen = nullptr;

	bool m_occMapGameOver = false;
	bool m_didPlayerWinOccGame = false;
	AABB2 m_objsTocollectBox;
	AABB2 m_tenDigitbox;
	AABB2 m_onesDigitBox;
	AABB2 m_occGameOverBox;
	AABB2 m_occwinbox;

	Texture* m_objsToCollectTex = nullptr;
	Texture* m_occGameOverTex = nullptr;
	Texture* m_occGameWinTex = nullptr;
	Texture* m_oneTex = nullptr;
	Texture* m_zeroTex = nullptr;
	Texture* m_twoTex = nullptr;
	Texture* m_threeTex = nullptr;
	Texture* m_fourTex = nullptr;
	Texture* m_fiveTex = nullptr;
	Texture* m_sixTex = nullptr;
	Texture* m_sevenTex = nullptr;
	Texture* m_eightTex = nullptr;
	Texture* m_nineTex = nullptr;

	int m_numObjectsToCollect = 0;


	SpriteAnimDefinition* m_sample;

	SpriteAnimDefinition* m_aiAnimWalk1;
	SpriteAnimDefinition* m_aiAnimWalk2;
	SpriteAnimDefinition* m_aiAnimWalk3;
	SpriteAnimDefinition* m_aiAnimWalk4;
	SpriteAnimDefinition* m_aiAnimWalk5;
	SpriteAnimDefinition* m_aiAnimWalk6;
	SpriteAnimDefinition* m_aiAnimWalk7;
	SpriteAnimDefinition* m_aiAnimWalk8;
	SpriteAnimDefinition* m_aiAnimWalk9;

	SpriteAnimDefinition* m_aiAnimIdle1;
	SpriteAnimDefinition* m_aiAnimIdle2;
	SpriteAnimDefinition* m_aiAnimIdle3;
	SpriteAnimDefinition* m_aiAnimIdle4;
	SpriteAnimDefinition* m_aiAnimIdle5;
	SpriteAnimDefinition* m_aiAnimIdle6;
	SpriteAnimDefinition* m_aiAnimIdle7;
	SpriteAnimDefinition* m_aiAnimIdle8;
	SpriteAnimDefinition* m_aiAnimIdle9;

	SpriteAnimDefinition* m_aiAnimMeleeAttack1;
	SpriteAnimDefinition* m_aiAnimMeleeAttack2;
	SpriteAnimDefinition* m_aiAnimMeleeAttack3;
	SpriteAnimDefinition* m_aiAnimMeleeAttack4;
	SpriteAnimDefinition* m_aiAnimMeleeAttack5;
	SpriteAnimDefinition* m_aiAnimMeleeAttack6;
	SpriteAnimDefinition* m_aiAnimMeleeAttack7;
	SpriteAnimDefinition* m_aiAnimMeleeAttack8;
	SpriteAnimDefinition* m_aiAnimMeleeAttack9;

	SpriteAnimDefTex* m_greenBeretActiveAnims = nullptr;
	SpriteAnimDefTex* m_greenBeretDeactiveAnims = nullptr;

	float m_time = 0.f;

	std::vector<MonsterAI*> m_enemies;
	std::vector<Bomb*> m_bombs;
	std::vector<Turret*> m_turrets;
	std::vector<OccAI*> m_occAIs;
	SymmetricPotentialField* m_symmetricField;

	bool StartLocationSet = false;
	bool endLocationSet = false;
	Vec2 startLocation;
	Vec2 endLocation;

	bool m_gamePaused = false;

	std::vector<int> pathIndices;

	GreenBeret* m_greenBeret;

public:

	Camera* m_gameCamera;
	Camera* m_devConsoleCamera;
	Camera* m_hudCamera;
	Vec2 m_mousePosition;
	Camera* m_occHudCamera;

	PotentialFieldCreator* m_potCreator = nullptr;

	//Player Anims
	SpriteAnimDefTex* m_player1IdleMeleeTex = nullptr;
	SpriteAnimDefTex* m_player1WalkMeleeTex = nullptr;
	SpriteAnimDefTex* m_player1AttackTex = nullptr;

	SpriteAnimDefTex* m_player1IdleGunTex = nullptr;
	SpriteAnimDefTex* m_player1WalkGunTex = nullptr;
	SpriteAnimDefTex* m_player1AttackGunTex = nullptr;

	//Support Player Anims
	SpriteAnimDefTex* m_supportPlayerIdleTex = nullptr;
	SpriteAnimDefTex* m_supportPlayerAttackTex = nullptr;
	SpriteAnimDefTex* m_supportPlayerWalkTex = nullptr;

	SpriteAnimDefTex* m_supportPlayerDisguiseWalk = nullptr;
	SpriteAnimDefTex* m_supportPlayerDisguiseAttack = nullptr;

	Player* m_player;

	SupportPlayer* m_supportPlayer;
	std::vector<Tile> m_tiles;
	std::vector<Tile> m_mainMapTiles;
	std::vector<Tile> m_infAndPfTiles;


	IntVec2 m_mapSize = IntVec2(160,90);

	IntVec2 m_infAndPfGameSize = IntVec2( 80 , 45 );

	std::vector<Vertex_PCU> m_sandTiles;
	std::vector<Vertex_PCU> m_sandLeftTiles;
	std::vector<Vertex_PCU> m_sandRightTiles;
	std::vector<Vertex_PCU> m_sandTopTiles;
	std::vector<Vertex_PCU> m_sandDownTiles;
	std::vector<Vertex_PCU> m_sandTopRightTile;
	std::vector<Vertex_PCU> m_sandTopLeftTile;
	std::vector<Vertex_PCU> m_sandBottomLeftTile;
	std::vector<Vertex_PCU> m_sandBottomRightTile;



	std::vector<Vertex_PCU> m_blockTiles;

	std::vector<Vertex_PCU> sampleTile;

	Texture* m_sandMainTex = nullptr;
	Texture* m_borderLeftTex = nullptr;
	Texture* m_borderRightTex = nullptr;
	Texture* m_borderBotTex = nullptr;
	Texture* m_borderTopTex = nullptr;

	Texture* m_blockTex = nullptr;

	Texture* sampleTex = nullptr;

	bool m_cameraUpdates = true;

	bool m_mainPlayerActive = true;
	
	bool devConsoleOpen = false;

	bool m_currentAlgIs4WayAStar = true;
	bool m_currentAlgIs8WayAstar = false;
	bool m_considerInfluenceMaps = false;

	RandomNumberGenerator* m_rng = nullptr;

	std::vector<PathFindingHelper> openList1;
	std::vector<PathFindingHelper> closedList1;

	std::vector<OccMapCollectable*> m_collectables;

	bool pathFound = false;

	bool initDone = false;

	double influnceMapTime = 0.0;

	bool inStartScreen = true;

	AABB2 m_hudBox;
	AABB2 m_playerBox;
	AABB2 m_player1Box;
	AABB2 m_player2Box;
	Texture* m_player1HudTex = nullptr;
	Texture* m_player2HudTex = nullptr;
	Texture* m_HudBoxTex = nullptr;
	Texture* m_PlayerBoxTex = nullptr;

	SpriteAnimDefinition* m_bombIdleTex = nullptr;
	SpriteAnimDefinition* m_explosion = nullptr;
	Texture* m_turretTex = nullptr;

	bool influenceMapPositive = false;


	AABB2 m_mapPart1 = AABB2( 0.f , 0.f , 320.f , 720.f );
	
	Texture* m_mapTex1 = nullptr;

	IntVec2 m_mainMapSize = IntVec2( 320 , 720 );

	MapCreator* m_mapCretor = nullptr;

	PotentialField* m_potentialField = nullptr;

	Timer* m_greenBeretHUDTimer = nullptr;

	IntVec2 m_occMapGameSize = IntVec2( 80 , 45 );

	std::vector<Tile> m_occMapTiles;

	bool m_inOccCreation = true;

	BitmapFont* m_font = nullptr;

};