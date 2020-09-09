#pragma once
#include <vector>
#include "GameCommon.hpp"
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

class Game
{

public:

	Game();
	~Game();


	void Update( float deltaseconds );
	void Render();
	void LoadPlayerTextures();
	void LoadSupportPlayerTextures();

	void UpdateCamera();
	void ToggleCameraUpdate();
	void UpdateMousePosition();
	void TogglePlayers();
	
	void PopulateTiles();
	void HandleBlockCollissions(Entity* entity);
	bool IsTileSolid( IntVec2 tileCoords );

	int GetTileIndexForTileCoords( const IntVec2& tileCoords );
	void MapFillUsingWorms( TileTextureType type, int minWorkLength = 4, int maxWorkLength =12 , int maxWorms = 90);


public:

	Camera* m_gameCamera;
	Vec2 m_mousePosition;

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
};