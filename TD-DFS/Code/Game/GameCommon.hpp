#pragma once
//#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
//#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Audio/AudioSystem.hpp"

class Window;
class InputSystem;
class RenderContext;
class AudioSystem;
class App;

constexpr float CLIENT_ASPECT = 16.f/9.f; 
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern Window* g_theWindow;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;


enum RenderLayers
{
	LAYER_1,
	LAYER_2,
	LAYER_3,
	LAYER_4,
	LAYER_5,
	LAYER_6,
	LAYER_7,
	LAYER_8,
};

enum Faction
{
	FACTION_GOOD ,
	FACTION_BAD
};

enum Upgrade
{
	UPGRADE_NONE = 0 ,
	UPGRADE_SMALL ,
	UPGRADE_MEDIUM ,
	UPGRADE_LARGE
};

enum Lanes
{
	LANE_1,
	LANE_2
};

