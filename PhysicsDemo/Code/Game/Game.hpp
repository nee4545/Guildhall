#pragma once
#include "GameCommon.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <vector>
#include "Engine/Core/NamedProperties.hpp"


class BitmapFont;
class Physics2D;
class GameObject;
class Poygon2D;
class BitmapFont;

static Physics2D* physicsSystem = nullptr;

class Dummy
{
public:
	bool something( EventArgs& args )
	{
		return true;
	}
};

class Game
{

public:

	Game();
	~Game();

	void StartUp();
	void Update( float deltaseconds );
	void Render();
	void BeginFrame();
	void EndFrame();

	GameObject* CreateDisc();
	GameObject* CreatePolygon( Polygon2D& polygon );
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
	bool IsPolygonPotentiallyConvex( Vec2 newAddedPoint );
	void HandlePolygonDrawMode();
	void DrawModeRender();
	void HandleObjectsOutOfBounds();
	void HandleClockChanges();
	void DisplayMiscInfo();
	void DisplayX();
	void HandleGravityModification();
	void DisplayToolTip();
	void UpdateFramePositions();
	void ToggleDevConsole();

	Dummy d;

private:
	RandomNumberGenerator m_rng;
	BitmapFont* m_BitmapFont = nullptr;
	Camera* m_camera=nullptr;
	Camera* m_devConsoleCamera = nullptr;
	AABB2 m_screenBounds=AABB2( Vec2( -80.f , -45.f ) , Vec2( 80.f , 45.f ) );
	
	std::vector<GameObject*> m_gameObjects;
	GameObject* m_selectedObject = nullptr;
	Vec2 m_mousePosition;
	float m_cameraMovementSpeed = 20.f;
	bool m_dragInProgress = false;

	std::vector<Vec2> drawModePoints;
	bool isDrawing = false;
	Vec2 invalidPoint;
	bool isThereInvalidPoint = false;
	int m_frameCounter = 0;
	bool m_offsetSet = false;
	Vec2 m_offset;
	float m_cameraHeight = 0.f;
	float m_zoomspeed = 100.f;
	float m_maxZoomIn = 0.f;
	float m_maxZoomOut = 0.f;

	Vec2 throwInitialPoint;
	bool initialPointSet = false;
	Vec2 throwFinalPoint;
	bool finalPointSet = false;
	bool devConsoleOpen = false;



};