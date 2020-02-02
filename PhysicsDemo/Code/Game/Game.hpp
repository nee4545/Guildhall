#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>


class BitmapFont;
class Physics2D;
class GameObject;

class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	Physics2D* m_physicsSystem = nullptr;
	std::vector<GameObject*> m_gameObjects;
	GameObject* selectedObject = nullptr;
	Vec2 m_mousePosition;

public:
	

	
	Game();
	void StartUp();
	GameObject* CreateDisc();
	void UpdateMousePosition();


	void Update(float deltaseconds);
	void Render();


	
};