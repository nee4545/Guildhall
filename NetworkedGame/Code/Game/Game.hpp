#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class Camera;
class InputSystem;

class Game
{
public:
	Game();
	~Game();

	virtual void StartUp();
	virtual void Update(float deltaseconds);
	virtual void Render();
	InputSystem* GetInputSystem();

	InputSystem* m_InputSystem = nullptr;
	bool isSinglePlayer = true;
};