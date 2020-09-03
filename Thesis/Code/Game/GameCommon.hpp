#pragma once
//#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
//#include "Engine/Input/InputSystem.hpp"
//#include "Engine/Audio/AudioSystem.hpp"

class Window;
class RenderContext;
class InputSystem;
class AudioSystem;
class Game;

constexpr float CLIENT_ASPECT = 2.f/1.f; 
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

