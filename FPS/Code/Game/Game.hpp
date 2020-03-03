#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


class BitMapFont;
class Texture;
class Shader;
class GPUMesh;



class Game
{
	RandomNumberGenerator rng;
	Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	float m_rColorValue = 0.f;
	BitmapFont* m_font;
	bool devConsoleOpen = false;
	Texture* tex = nullptr;
	GPUMesh* mesh = nullptr;

public:

	Vec3 movement;
	Vec3 m_cameraRotation;
	Game();
	~Game();

	bool toggleCamera = false;
	bool paused = false;
	bool isDebugging = false;

	void Update( float deltaseconds );
	void Render();

	void UpdateCamera();

	void ToggleDebugCamera();
	void ToggleDevConsole();
	void ToggleDebugging();











};