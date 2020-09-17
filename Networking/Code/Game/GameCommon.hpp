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
class NetworkSystem;
class ImGuiSystem;

constexpr float CLIENT_ASPECT = 16.f/9.f; 
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern Window* g_theWindow;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;
extern Game* thegame;
extern NetworkSystem* g_theNetwork;
extern ImGuiSystem* g_theGUI;

enum eBillBoardType
{
	CAMERA_FACING_XY ,
	CAMERA_OPPOSED_XY ,
	CAMERA_OPPOSED_XYZ ,	// Screen Aligned.
	CAMERA_FACING_XYZ ,
};

constexpr float PLAYER_SPEED = 5.f;