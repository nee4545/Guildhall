#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>


class BitmapFont;
class Physics2D;
class GameObject;
class Poygon2D;

class Game
{
	RandomNumberGenerator m_rng;
	Camera* m_camera=nullptr;
	Physics2D* m_physicsSystem = nullptr;
	std::vector<GameObject*> m_gameObjects;
	GameObject* m_selectedObject = nullptr;
	Vec2 m_mousePosition;
	float m_cameraMovementSpeed = 20.f;
	bool m_dragInProgress = false;

	std::vector<Vec2> drawModePoints;
	bool isDrawing = false;
	Vec2 invalidPoint;
	bool isThereInvalidPoint = false;

public:
	

	
	Game();
	void StartUp();
	void EndFrame();
	GameObject* CreateDisc();
	GameObject* CreatePolygon(Polygon2D& polygon);
	void PopulateInitialObjects();
	void UpdateCameraMovement( float deltaSeconds );
	void HandleMouseInsideObjects();
	void HandleCollissions();
	void HandleDrag();
	void HandleObjectCreationRequests();
	void HandleThrow();
	void ResetCamera();
	void GetCurrentSelectedObject();
	void ZoomInCamera( float deltaSeconds );
	void ZoomOutCamera( float deltaSeconds );
	void PolygonDrawMode();
	bool IsPolygonPotentiallyConvex(Vec2 newAddedPoint);
	void HandlePolygonDrawMode();
	void DrawModeRender();

	void Update(float deltaseconds);
	void Render();

	bool m_offsetSet = false;
	Vec2 m_offset;
	float m_cameraHeight = 0.f;
	float m_zoomspeed = 1000.f;
	float m_maxZoomIn = 0.f;
	float m_maxZoomOut = 0.f;

	Vec2 throwInitialPoint;
	bool initialPointSet = false;
	Vec2 throwFinalPoint;
	bool finalPointSet = false;
	
};