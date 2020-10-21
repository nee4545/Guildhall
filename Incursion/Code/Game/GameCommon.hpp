#pragma once
#include "Engine/Renderer/RenderContext.hpp"
//#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
//#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Audio/AudioSystem.hpp"

class Window;
class RenderContext;
class InputSystem;
class AudioSystem;
class Game;

constexpr float CLIENT_ASPECT = 2.f / 1.f;
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern Window* g_theWindow;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Game* thegame;

enum eBillBoardType
{
	CAMERA_FACING_XY ,
	CAMERA_OPPOSED_XY ,
	CAMERA_OPPOSED_XYZ ,	// Screen Aligned.
	CAMERA_FACING_XYZ ,
};



constexpr int WORLD_SIZE_X = 20;
constexpr int WORLD_SIZE_Y = 30;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float BULLET_SPEED = 3.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;

constexpr int MAX_DEBRIS= 1000;
constexpr float MAX_CAMERA_SHAKE=10.f;

constexpr float UI_XMAX=1600.f;
constexpr float UI_YMaX=800.f;

constexpr float PERCENTAGE_OF_OBSTACLE=0.3f;
constexpr float PERCENTAGE_OF_MUDTILES=0.1f;

constexpr int MAX_PLAYER_HEALTH=3;
constexpr int MAX_NPC_TANK_HEALTH=3;
constexpr int MAX_NPC_TURRET_HEALTH=3;
constexpr float PLAYER_TANK_SPEED = 1.f;

constexpr int MAX_WORMS=12;
constexpr int MIN_WORM_LENGTH =2;
constexpr int MAX_WORM_LENGTH = 7;




