#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>


class BitmapFont;
class Physics2D;
class GameObject;

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

public:
	

	
	Game();
	void StartUp();
	void EndFrame();
	GameObject* CreateDisc();
	void PopulateInitialObjects();
	void UpdateCameraMovement( float deltaSeconds );
	void HandleMouseInsideObjects();
	void HandleCollissions();
	void HandleDrag();
	void HandleObjectCreationRequests();
	void ResetCamera();
	void GetCurrentSelectedObject();
	void ZoomInCamera( float deltaSeconds );
	void ZoomOutCamera( float deltaSeconds );

	void Update(float deltaseconds);
	void Render();

	bool m_offsetSet = false;
	Vec2 m_offset;
	float m_cameraHeight = 0.f;
	float m_zoomspeed = 1000.f;
	float m_maxZoomIn = 0.f;
	float m_maxZoomOut = 0.f;
	
};