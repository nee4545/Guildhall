#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/AABB2.hpp"
class Camera;
class Texture;
class PotentialField;

class PotentialFieldCreator
{
public:
	PotentialFieldCreator();
	~PotentialFieldCreator();

public:
	Camera* m_camera;
	float m_spriteSize = 2.f;
	Vec2 m_forwardVector = Vec2( 0.f , 1.f );
	float m_sectorRadius = 5.f;
	float m_sectorAngle = 30.f;
	AABB2 m_spriteAABB;
	Texture* m_spriteTex = nullptr;
	Vec2 m_spritePosition;
	Vec2 m_mousePos;
	PotentialField* m_potentialField = nullptr;

	int m_potentialFieldRadius = 5;

	Vec2 cameraPosition;
	int zoomFactor = 1;

	bool isCore = false;
	bool isConsidered = false;
	float angleDegrees = 0.f;


	void LoadPotentialFieldFromXml();
	void UpdateCameraMovement();
	void Update();
	void UpdateSprite();
	void Render();
	void Save();
	void UpdatePotentialFieldValues();
	int GetIndexInPotentialField(Vec2 pos);
	int currentChosenIndex = -1;
	


};