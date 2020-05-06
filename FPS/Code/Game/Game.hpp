#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


constexpr int MAX_SHADERS = 6;

class BitMapFont;
class Texture;
class Shader;
class GPUMesh;
class Material;

enum shaderType
{
	LIT, 
	NORMAL,
	TANGENT,
	BITANGENT,
	SURFACE_NORMAL,
	TOTAL
};


struct Fresnal
{
	Vec3 fresnalColor;
	float  fresnalPower;
	Vec3 pad00;
	float  fresnalFactor;
};

struct Dissolve
{
	Vec3 burnStartColor;
	float  burnEdgeWidth;
	Vec3 burnEndColor;
	float  burnAmount;
};

enum ColorTone
{
	NOTINT,
	GRAY,
	REDBRON,

	TOTAL_TONEMAPS,
};

class Game
{
	RandomNumberGenerator rng;
	mutable Camera* m_camera = nullptr;
	Camera* m_devConsoleCamera = nullptr;
	Camera* m_UICamera = nullptr;
	float m_rColorValue = 0.f;
	BitmapFont* m_font;
	bool devConsoleOpen = false;

	Texture* tex = nullptr;
	Texture* tileDiffuse = nullptr;
	Texture* tileNormal = nullptr;
	Texture* noise = nullptr;

	Material* dissolveMaterial = nullptr;

	GPUMesh* mesh = nullptr;
	GPUMesh* sphere = nullptr;
	GPUMesh* quad = nullptr;
	Transform cubeTransform;
	Transform quadTransform;
	Transform sphereTransform;

	int currentShaderNumber = 0;
	int currentAttenuationNumber = 0;
	bool lightShouldAnimate = false;
	bool lightFollowCamera = false;

	float ambientLightIntensity = 0.3f;
	float specularPower = 32.f;
	float specularFactor = 1.f;

	Vec3 specularAttenuation = Vec3( 0.f , 0.f , 1.f );
	float roataionAngleForAnimation = 0.f;
	Fresnal fresnalData;
	Dissolve dissolveData;
	float currentBurnAmt = 0.f;
	Mat44 m_toneMapTransform;
	ColorTone m_currentToneMap = ColorTone::NOTINT;

public:

	Vec3 movement;
	Vec3 m_cameraRotation;
	int renderMode = 2;
	Game();
	~Game();

	bool toggleCamera = false;
	bool paused = false;
	bool isDebugging = false;
	bool isBloomOn = false;
	bool isColorToneOn = false;

	void Update( float deltaseconds );
	void Render();

	void UpdateCamera();

	void ToggleDebugCamera();
	void ToggleRenderModes();
	void ToggleDevConsole();
	void ToggleDebugging();
	void ToggleShaders();
	void ToggleLightPositions();
	void ToggleAmbientLightIntensity();
	void ToggleSpecularPower();
	void ToggleSpecularFactor();
	void ToggleSpecularAttenuation();
	void DisplayUIText();
	void ToggleLightIntensity();
	void UpdateDissolveDetails();
	void ToggleLights();
	void ToggleLightTypes();
	void ToggleBloom();
	void ToggleColorTone();
};