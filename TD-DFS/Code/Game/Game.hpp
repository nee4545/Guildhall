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
	RandomNumberGenerator m_rng;
	mutable Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Camera* m_UICamera = nullptr;
	float m_rColorValue = 0.f;
	BitmapFont* m_font;
	bool devConsoleOpen = false;


	std::vector<Vertex_PCU> m_basisVerts;
	std::vector<Hero*> m_Heros;

	SoundID testSound;

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
	void LoadAnimations();
	
	void CreateMeleeHeros( Faction faction , Upgrade upg , Lanes lane);
};