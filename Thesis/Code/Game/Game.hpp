#pragma once
#include <vector>
#include "GameCommon.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Audio/AudioSystem.hpp"


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

enum GameMode
{
	GAME,
	PATHFINDER,
	MAPCREATOR,
	POTENTIALFIELD_CREATOR,
	
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
	
	void PopulateTiles();
	void HandleBlockCollissions(Entity* entity);
	bool IsTileSolid( IntVec2 tileCoords );
	
	void GetPathUsingAStarIgnoreDiagonalMovesOneStep(Vec2 startPos, Vec2 endPos,std::vector<int>& path, bool ignoreDiagonalMoves = false, bool considerInfluenceMaps = false);
	void GetPathUsingAstarWithDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path );
	void GetPathUsingAStarIgnoreDiagonalMoves( Vec2 startPos , Vec2 endPos , std::vector<int>& path , bool considerInfuenceMap = false , bool ignoreDiagonalMoves = false );
	float GetManhattanDistance( IntVec2 currentPos , IntVec2 finalPos );
	void CreateInfluenceMap( IntVec2 startCoords , bool isPositive , int initialValue );

	void GetAllTilesWithSameFCost( std::vector<PathFindingHelper>& list , std::vector<PathFindingHelper>& outList );
	bool IsPathFindingHelpInList( PathFindingHelper toFind , std::vector<PathFindingHelper>& list );

	void ToggleDevConsole();

	int GetTileIndexForTileCoords( const IntVec2& tileCoords );
	int GetTileIndexForTileCoordsForMainMap( const IntVec2& tileCoords );
	void MapFillUsingWorms( TileTextureType type, int minWorkLength = 4, int maxWorkLength =12 , int maxWorms = 90);

	void LoadDataFromXml();
	void LoadPotentialFieldFromXML();

	GameMode m_currentMode = PATHFINDER;
	StartScreen* m_startScreen = nullptr;

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

	bool StartLocationSet = false;
	bool endLocationSet = false;
	Vec2 startLocation;
	Vec2 endLocation;

	std::vector<int> pathIndices;

	GreenBeret* m_greenBeret;

public:

	Camera* m_gameCamera;
	Camera* m_devConsoleCamera;
	Camera* m_hudCamera;
	Vec2 m_mousePosition;

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


	IntVec2 m_mapSize = IntVec2(160,90);


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

};