#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class Map;
class SpriteSheet;
class BitmapFont;

class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera=nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Map* m_map;
	BitmapFont* m_BitmapFont =nullptr;
	
	bool m_isConsoleOpen=false;
	bool m_isCameraUpdaing = true;
public:

	
	
	Game();
	void StartUp();
	void Update(float deltaseconds);
	void Render();
	void ShutDown();
	
	void UpdateCamera();
	void ToggleDevConsole();
	void ToggleDebugCamera();
	void TogglePaused();
	void ToggleDebugging();
	void DisplayMouseInspectorDetails();
	

	
	





	
	
};