#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class Camera;

class Game
{
public:
	Game();
	~Game();

	Camera* m_camera=nullptr;
	Camera* m_UICamera= nullptr;

	void Update(float deltaseconds);
	void Render();
};