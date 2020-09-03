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