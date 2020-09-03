#pragma once
#include <vector>
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class GPUMesh;
class TileMap;
class SpriteSheet;
class World;
class Entity;

class Game
{
	friend class World;
	friend class Entity;
	friend class Actor;
	friend class TileMap;
	friend class Portal;

	RandomNumberGenerator m_rng;
	mutable Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Camera* m_UICamera = nullptr;
	Camera* m_HUDCamera = nullptr;
	float m_rColorValue = 0.f;
	BitmapFont* m_font;
	bool devConsoleOpen = false;

	float m_pitch = 0.f;
	float m_yaw = 0.f;
	float m_roll = 0.f;

	bool m_debugDrawOn = false;
	
	GPUMesh* m_cube;
	GPUMesh* m_basis;
	Transform cubeTransform1;
	Transform cubeTransform2;
	Transform cubeTransform3;

	Entity* m_player = nullptr;

	std::vector<Vertex_PCU> m_basisVerts;

	SoundID testSound;
	int m_fps = 0;
	SpriteSheet* m_weaponSprites;

public:

	Game();
	~Game();


	void Update( float deltaseconds );
	void Render();

	void UpdateCamera();
	void ToggleDebugCamera();
	void ToggleRenderModes();
	void ToggleDevConsole();
	void ToggleDebugging();
	void DisplayUI();
	void DisplayFPS();
	void DisplayHUD();
	void DisplayDebugInfo();
	void SoundTest();

	void UpdatePossesingEntityFromKeyBoard();


	TileMap* testMap = nullptr;
	World* m_world;
};