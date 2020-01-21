#pragma once
#include "GameCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
class PlayerShip;
class Bullet;
class Astroid;
class Entity;
class Bettle;
class Wasp;
class Debris;


class Game
{
	PlayerShip* playerShip;
	Bullet* bullets[MAX_BULLETS]={nullptr};
	Astroid* astroids[MAX_ASTEROIDS]={nullptr};
	Bettle* bettle[MAX_BUGS]={nullptr};
	Wasp* wasp[MAX_BUGS]={nullptr};
	Debris* debris[MAX_DEBRIS]={nullptr};
	

	float m_intensity=0.f;
	int m_currentlevel=0;
	int max_levles=5;
	int lives=4;
	float timeTowait=3.f; //time to wait after the player dies and lost all lives
	float vibration_intensity=0.f;

	RandomNumberGenerator rn;
	
public:
	
	Game();

	bool canShoot=true;
	bool canSpawnAstroid=true;
	bool isDebugging=false;

	bool attractMode=true;

	void Update(float deltaseconds);

	void Render();

	void SpawnBullet(Vec2 postiton, float orientationDegrees);

	void SpawnBulletJoyStick(Vec2 position, float orientationDegress);

	void SpawnInitialAstroids();

	void ResetShip();

	void SpawnAstroidFromKeyboard();

	void SpawnAstroid();

	bool IsOffscreen(const Entity* entity );

	void FindAndMakeGarbageEntities();

	void DeleteGarbageEntities();

	void HandelKeyEvents();

	bool DoEntitiesOverlap(const Entity* a, const Entity* b) const;
	
	void DetectCollissionsBtweenAstroidsAndBullets();

	void DetectCollissionBtweenBugsAndBullets();

	void DetectCollissionBtweenShipAndAstroid();

	void DetectCollissionBtweenShipAndBugs();

	void DrawLine(const Vec2 &start,const Vec2 &end, const Rgba8 &color, float thickness);

	void DrawRings(const Entity* entity);

	void DrawRingsForAllEnitiesAlive();

	void DrawLinesFromShipToAllEntitiesAlive();

	void DrawVelocityLine();

	void ToggleDebugging();

	int GetBulletCount();

	int GetAstroidCount();

	Vec2 GetPlayerShipPosition();

	void SpawnWaves();

	int GetAliveEnititesCount();

	void SpawnDebris(int numberOfDebris, Rgba8 debris_color, Vec2 position);

	void CamerShake(float intensity);
	
	void SpawnBettle();

	void SpawnWasp();

	bool GetPlayerShipStatus();

	void RenderUI();

	void DisplayAttractScreen();

	void DeleteAllAliveEntities();

	void HandleStateEvents();

	Vec2 GetRandomLocationJustOffScreen();

	void VibrateJoystick(float deltaseconds);
	
};