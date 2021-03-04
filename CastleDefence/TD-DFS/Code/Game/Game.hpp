#pragma once
#include <vector>
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class GPUMesh;
class SpriteAnimDefTex;
class Hero;
class Timer;
class Clock;
class HUD;
class SelectedHUD;
class RangeHero;
class ShootObject;
class StartScreen;
class AIController;
class LoadingScreen;
class UncontrolledHero;
class Tower;
class TowerDestroyer;
class TowerBullet;

const std::string MELLEE_UPGRADE_1_TEXT = "";
const std::string MELLEE_UPGRADE_2_TEXT = "";
const std::string MELLEE_UPGRADE_3_TEXT = "";

const std::string RANGE_UPGRADE_1_TEXT = "";
const std::string RANGE_UPGRADE_2_TEXT = "";
const std::string RANGE_UPGRADE_3_TEXT = "";

const std::string TOWER_UPGRADE_1_TEXT = "++ ATTACK SPEED";
const std::string TOWER_UPGRADE_2_TEXT = "FIRE BULLETS";
const std::string TOWER_UPGRADE_3_TEXT = "ICE BULLETS";

const std::string TOWER_DESTROYER_UPGRADE_TEXT = "++ DURATION";


constexpr int MELEE_HERO_UPGRADE0_COST = 10;
constexpr int MELEE_HERO_UPGRADE1_COST = 40;
constexpr int MELEE_HERO_UPGRADE2_COST = 70;
constexpr int MELEE_HERO_UPGRADE3_COST = 100;

constexpr int RANGE_HERO_UPGRADE0_COST = 10;
constexpr int RANGE_HERO_UPGRADE1_COST = 40;
constexpr int RANGE_HERO_UPGRADE2_COST = 70;
constexpr int RANGE_HERO_UPGRADE3_COST = 100;

constexpr int TOWER_UPGRADE0_SPAWN_COST = 10;
constexpr int TOWER_UPGRADE1_SPAWN_COST = 40;
constexpr int TOWER_UPGRADE2_SPAWN_COST = 70;
constexpr int TOWER_UPGRADE3_SPAWN_COST = 100;

constexpr int MELEE_UPGRADE0_COST = 20;
constexpr int MELEE_UPGRADE1_COST = 40;
constexpr int MELEE_UPGRADE2_COST = 70;
constexpr int MELEE_UPGRADE3_COST = 100;

constexpr int RANGE_UPGRADE0_COST = 20;
constexpr int RANGE_UPGRADE1_COST = 40;
constexpr int RANGE_UPGRADE2_COST = 70;
constexpr int RANGE_UPGRADE3_COST = 100;

constexpr int TOWER_UPGRADE0_COST = 20;
constexpr int TOWER_UPGRADE1_COST = 40;
constexpr int TOWER_UPGRADE2_COST = 70;
constexpr int TOWER_UPGRADE3_COST = 100;

constexpr int TOWER_DESTROYER0_COST = 30;
constexpr int TOWER_DESTROYER1_COST = 50;
constexpr int TOWER_DESTROYER_UPGRADE_COST = 70;

constexpr int MELEE_UPGRADE0_DEATH_COINS = 20;
constexpr int MELEE_UPGRADE1_DEATH_COINS = 50;
constexpr int MELEE_UPGRADE2_DEATH_COINS = 80;
constexpr int MELEE_UPGRADE3_DEATH_COINS = 110;

constexpr int RANGE_UPGRADE0_DEATH_COINS = 20;
constexpr int RANGE_UPGRADE1_DEATH_COINS = 50;
constexpr int RANGE_UPGRADE2_DEATH_COINS = 80;
constexpr int RANGE_UPGRADE3_DEATH_COINS = 110;

constexpr int TOWER_UPGRADE0_DEATH_COINS = 20;
constexpr int TOWER_UPGRADE1_DEATH_COINS = 50;
constexpr int TOWER_UPGRADE2_DEATH_COINS = 80;
constexpr int TOWER_UPGRADE3_DEATH_COINS = 110;


enum PlayerActions
{
	ACTION_NONE ,
	ACTION_SELECT_MELEE ,
	ACTION_SELECT_RANGE ,
	ACTION_SELECT_TOWER ,
	ACTION_SELECT_TOWER_DESTOYER ,
	ACTION_UPGRADE_MELEE ,
	ACTION_UPGRADE_RANGE ,
	ACTION_UPGRADE_TOWER ,
	ACTION_UPGRADE_UHERO ,
	ACTION_UPGRADE_TOWER_DESTROYER,
};

struct PlayerStates
{
	Upgrade meleeHeroState = UPGRADE_NONE;
	Upgrade rangeHeroState = UPGRADE_NONE;
	Upgrade towerState = UPGRADE_NONE;
	Upgrade towerDestroyerState = UPGRADE_NONE;
};

struct LayeredObject
{
	Texture* tex;
	AABB2 object;
	bool isXFlipped = false;
	bool isYFlipped = false;
};

class Game
{
	friend class Hero;
	friend class ShootObject;
	friend class AIController;
	friend class StartScreen;
	friend class UncontrolledHero;
	friend class Tutorial;

	RandomNumberGenerator m_rng;
	mutable Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Camera* m_UICamera = nullptr;
	float m_rColorValue = 0.f;
	bool devConsoleOpen = false;
	LoadingScreen* m_loadingScreen = nullptr;
	int framecount = 0;

	

	Texture* m_castleHealthBaseLoadBar = nullptr;
	Texture* m_castleLoadBar = nullptr;

	AIController* m_enemy = nullptr;

	std::vector<Vertex_PCU> m_basisVerts;

public:

	float m_castleHealth = 200.f;
	float m_enemyCastleHealth = 200.f;
	BitmapFont* m_font;

	std::vector<Hero*> m_playerMeleeHerosLane1_1;
	std::vector<Hero*> m_playerMeleeHerosLane1_2;
	std::vector<Hero*> m_playerMeleeHerosLane2_1;
	std::vector<Hero*> m_playerMeleeHerosLane2_2;
	std::vector<Hero*> m_enemyMeleeHerosLane1_1;
	std::vector<Hero*> m_enemyMeleeHerosLane1_2;
	std::vector<Hero*> m_enemyMeleeHerosLane2_1;
	std::vector<Hero*> m_enemyMeleeHerosLane2_2;

	std::vector<ShootObject*> m_shootObjectsLane1_1;

	std::vector<RangeHero*> m_playerRangeHerosLane1_1;
	std::vector<RangeHero*> m_playerRangeHerosLane1_2;
	std::vector<RangeHero*> m_playerRangeHerosLane2_1;
	std::vector<RangeHero*> m_playerRangeHerosLane2_2;
	std::vector<RangeHero*> m_enemyRangeHerosLane1_1;
	std::vector<RangeHero*> m_enemyRangeHerosLane1_2;
	std::vector<RangeHero*> m_enemyRangeHerosLane2_1;
	std::vector<RangeHero*> m_enemyRangeHerosLane2_2;


public:
	std::vector<UncontrolledHero*> m_UplayerLane1_1;
	std::vector<UncontrolledHero*> m_UplayerLane1_2;
	std::vector<UncontrolledHero*> m_UenemyLane1_1;
	std::vector<UncontrolledHero*> m_UenemyLane1_2;
	std::vector<UncontrolledHero*> m_UplayerLane2_1;
	std::vector<UncontrolledHero*> m_UenemyLane2_1;

	std::vector<TowerDestroyer*> m_towerDestroyers; 
	std::vector<TowerBullet*> m_towerBullets;

	Timer* m_UncontollerHeroSpawnTimer = nullptr;
	Clock* m_UncontollerHeroSpawnClock = nullptr;

	SoundID testSound;
	Texture* m_guiBackround;

	int m_playerCoins = 500;
	int m_enemyCoins = 500;

	int m_currentMeleeHeroCost = 0;
	int m_currentRangeHeroCost = 0;
	int m_currentTowerCost = 0;
	int m_currentTowerDestroyerCost = 0;

	//Textures
public:

	PlayerActions m_playerAction = ACTION_NONE;
	PlayerStates m_playerUpgradeState;
	PlayerStates m_enemyUpgradeState;

	AABB2 m_lane1;
	AABB2 m_lane2;

	AABB2 m_enemytdlane1;
	AABB2 m_enemytdlane2;

	AABB2 m_playertdlane1;
	AABB2 m_playertdlane2;

	std::vector<LayeredObject> m_Layer1Objects;
	std::vector<LayeredObject> m_Layer2Objects;
	std::vector<LayeredObject> m_Layer3Objects;
	std::vector<LayeredObject> m_Layer4Objects;
	std::vector<LayeredObject> m_Layer5Objects;
	AABB2 m_UIAreaBot;
	AABB2 m_UIAreaTop;

	StartScreen* m_startscreen=nullptr;

	SpriteAnimDefTex* m_PlayerUpgrade0MeleeRun;
	SpriteAnimDefTex* m_PlayerUpgrade0MeleeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade1MeleeRun;
	SpriteAnimDefTex* m_PlayerUpgrade1MeleeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade2MeleeRun;
	SpriteAnimDefTex* m_PlayerUpgrade2MeleeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade3MeleeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade3MeleeRun;

	SpriteAnimDefTex* m_PlayerUpgrade0RangeRun;
	SpriteAnimDefTex* m_PlayerUpgade0RangeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade1RangeRun;
	SpriteAnimDefTex* m_PlayerUpgade1RangeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade2RangeRun;
	SpriteAnimDefTex* m_PlayerUpgrade2RangeAttack;
	SpriteAnimDefTex* m_PlayerUpgrade3RangeRun;
	SpriteAnimDefTex* m_PlayerUpgrade3RangeAttack;

	SpriteAnimDefTex* m_EnemyUpgrade0MeleeRun;
	SpriteAnimDefTex* m_EnemyUpgrade0MeleeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade1MeleeRun;
	SpriteAnimDefTex* m_EnemyUpgrade1MeleeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade2MeleeRun;
	SpriteAnimDefTex* m_EnemyUpgrade2MeleeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade3MeleeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade3MeleeRun;

	SpriteAnimDefTex* m_EnemyUpgrade0RangeRun=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade0RangeAttack=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade1RangeRun=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade1RangeAttack=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade2RangeRun=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade2RangeAttack=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade3RangeAttack=nullptr;
	SpriteAnimDefTex* m_EnemyUpgrade3RangeRun=nullptr;

	SpriteAnimDefTex* m_UPlayer0Run=nullptr;
	SpriteAnimDefTex* m_UPlayer0Attack=nullptr;
	SpriteAnimDefTex* m_UPlayer1Run=nullptr;
	SpriteAnimDefTex* m_UPlayer1Attack=nullptr;

	SpriteAnimDefTex* m_UEnemy0Run=nullptr;
	SpriteAnimDefTex* m_UEnemy0Attack=nullptr;
	SpriteAnimDefTex* m_UEnemy1Run=nullptr;
	SpriteAnimDefTex* m_UEnemy1Attack=nullptr;

	Texture* m_RangerHero0PlayerBulletTex = nullptr;
	Texture* m_RangerHero1PlayerBulletTex = nullptr;
	Texture* m_RangerHero2PlayerBulletTex = nullptr;
	Texture* m_RangerHero3PlayerBulletTex = nullptr;

	Texture* m_RangerHero0EnemyBulletTex = nullptr;
	Texture* m_RangerHero1EnemyBulletTex = nullptr;
	Texture* m_RangerHero2EnemyBulletTex = nullptr;
	Texture* m_RangerHero3EnemyBulletTex = nullptr;

	SpriteAnimDefTex* m_TowerDestroyer1WalkTex = nullptr;
	SpriteAnimDefTex* m_TowerDestroyer1AttackTex = nullptr;
	SpriteAnimDefTex* m_TowerDestroyer2WalkTex = nullptr;
	SpriteAnimDefTex* m_TowerDestroyer2AttackTex = nullptr;

	AABB2 m_healthBarBase;
	AABB2 m_healthBar;
	AABB2 m_enemyHealthBarBase;
	AABB2 m_enemyHealthBar;
	
	bool gameOver = false;
	bool didPlayerWin = false;

	Texture* m_towerBase = nullptr;
	Texture* m_tower = nullptr;
	Texture* m_towerTurret = nullptr;

	Texture* m_bullet0tex = nullptr;
	Texture* m_bullet1tex = nullptr;
	Texture* m_bullet2tex = nullptr;
	Texture* m_bullet3tex = nullptr;

	Tower* m_playerTower1 = nullptr;
	Tower* m_playerTower2 = nullptr;
	Tower* m_enemyTower1 = nullptr;
	Tower* m_enemyTower2 = nullptr;

	AABB2 m_ptowerBase1Box;
	AABB2 m_ptowerBase2Box;
	AABB2 m_etowerBase1Box;
	AABB2 m_etowerBase2Box;

	AABB2 m_winBackRound;
	AABB2 m_gameOverBox;
	AABB2 m_winLoseBox;

	Texture* m_winBackTex = nullptr;
	Texture* m_gameOverTex = nullptr;
	Texture* m_winTex = nullptr;
	Texture* m_loseTex = nullptr;

	AABB2 temp;

	HUD* m_meleeHeroHud = nullptr;
	HUD* m_rangeHeroHud = nullptr;
	HUD* m_towerHud = nullptr;
	HUD* m_towerDestroyerHud = nullptr;

	SelectedHUD* m_selectedHUD = nullptr;
	
	bool isInStartScreen = true;

	Vec2 m_mousePosition;

	bool assetsLoaded = false;
	bool renderReached = false;

public:

	Game();
	~Game();


	void Update( float deltaseconds );
	void Render();
	void LoadTextures();

	void CheckForWin();

	void ToggleDevConsole();
	void DisplayUI();
	void SoundTest();
	void DisplayMouseInfo();
	void LoadPlayerMeleeAnimations();
	void LoadEnemyMeleeAnimations();
	void LoadPlayerRangeAnimations();
	void LoadEnemyRangeAnimations();
	void LoadPlayerBulletTextures();
	void LoadUncontrolledHeroTextures();
	void LoadTowerTextures();
	void LoadTowerDestroyerTextures();
	void LoadTowerBulletTextures();
	void SpawnUnControlledHeros();
	void UpdateHealthBars();

	void ReduceHealth( float amt, bool isEnemy = false );

	void UpdateCosts();

	void DisplayCoins();

	bool SpawnTower( Vec2 position, Faction faction, Lanes lane, Upgrade upg );
	void SpawnTowerDestroyer(Faction faction, Upgrade upg, Lanes lane);
	void SpawnToweBullet( Faction faction , Upgrade upg , Vec2 position , float orientationDegrees , bool shouldMoveForward );
	
	void MeleeHeroTest( Faction faction , Upgrade upg , Lanes lane);
	void RangeHeroTest( Faction faction , Upgrade upg , Lanes lane );
	void UHeroTest();
	void TDtest();
	void CreateShootObjects(Faction faction,Vec2 position, Texture* tex,Lanes lane);

	void CreateMeleeHero( Faction faction , Upgrade upg , Lanes lane );
	void CreateRangeHero( Faction faction , Upgrade upg , Lanes lane );

	void DeleteGarbageEntities();
	void CreateHUD();
	void DetectPlayerActions();
	void ExecutePlayerActions();

	void UpdateMousePosition();
	void DisplayFPS();

	SoundID success;
	SoundID error;
	SoundID BackRound;

};