#pragma once
#include <vector>
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/AABB2.hpp"

class GPUMesh;
class SpriteAnimDefTex;
class Hero;
class Timer;
class Clock;
class HUD;
class RangeHero;

struct LayeredObject
{
	Texture* tex;
	RenderLayers layer=LAYER_1;
	AABB2 object;
	bool isXFlipped = false;
	bool isYFlipped = false;
};

class Game
{
	friend class Hero;

	RandomNumberGenerator m_rng;
	mutable Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Camera* m_UICamera = nullptr;
	float m_rColorValue = 0.f;
	BitmapFont* m_font;
	bool devConsoleOpen = false;


	std::vector<Vertex_PCU> m_basisVerts;
	std::vector<Hero*> m_playerMeleeHeros;
	std::vector<Hero*> m_enemyHeros;
	std::vector<RangeHero*> m_playerRangeHeros;

	SoundID testSound;
	Texture* m_guiBackround;

	//Textures
public:
	std::vector<LayeredObject> m_Layer1Objects;
	std::vector<LayeredObject> m_Layer2Objects;
	std::vector<LayeredObject> m_Layer3Objects;
	std::vector<LayeredObject> m_Layer4Objects;
	std::vector<LayeredObject> m_Layer5Objects;
	AABB2 m_UIAreaBot;
	AABB2 m_UIAreaTop;

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

	SpriteAnimDefTex* m_EnemyUpgrade0RangeRun;
	SpriteAnimDefTex* m_EnemyUpgrade0RangeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade1RangeRun;
	SpriteAnimDefTex* m_EnemyUpgrade1RangeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade2RangeRun;
	SpriteAnimDefTex* m_EnemyUpgrade2RangeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade3RangeAttack;
	SpriteAnimDefTex* m_EnemyUpgrade3RangeRun;


	/*AABB2 MeleeHeroBox;
	AABB2 MeleeHeroCostBox;
	AABB2 MeleeHeroUpgradeBox;
	AABB2 MeleeHeroUpgradeCostBox;*/

	AABB2 temp;
	AABB2 a;

	
	HUD* m_meleeHeroHud = nullptr;
	HUD* m_rangeHeroHud = nullptr;


public:

	Game();
	~Game();


	void Update( float deltaseconds );
	void Render();
	void LoadTextures();

	void UpdateCamera();
	void ToggleDebugCamera();
	void ToggleRenderModes();
	void ToggleDevConsole();
	void ToggleDebugging();
	void DisplayUI();
	void SoundTest();
	void DisplayMouseInfo();
	void LoadPlayerMeleeAnimations();
	void LoadEnemyMeleeAnimations();
	void LoadPlayerRangeAnimations();
	void LoadEnemyRangeAnimations();
	
	void MeleeHeroTest( Faction faction , Upgrade upg , Lanes lane);
	void RangeHeroTest( Faction faction , Upgrade upg , Lanes lane );
	void DeleteGarbageEntities();
	void CreateHUD();
	void DetectPlayerActions();
};