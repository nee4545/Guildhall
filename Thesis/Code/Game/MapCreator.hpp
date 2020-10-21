#pragma once
#include <vector>
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
class Texture;
class SpriteSheet;
class Camera;
class Game;

struct MapItem
{
	Texture* m_texture = nullptr;
	AABB2 m_bounds;
};

class MapCreator
{
public:
	MapCreator(Game* game);
	
	void Update(float deltaSeconds);
	void Render();
	void Save();
	void UpdateCamera();
	void UpdateCameraForParts();
	
	Camera* m_camera;
	Game* m_game = nullptr;
	bool currentTileSolid = false;
	int currentQuadrant = 1;
	int currentSubQuadrant = 1;
	int mapPart = 1;
	Vec2 m_mousePos;

	bool ShowMapInParts = false;

	std::vector<int> m_solidTileIndices;
	std::vector<Vertex_PCU> tileVerts;
};