#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Window;

constexpr float CLIENT_ASPECT = 16.f/9.f; 
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern Window* g_theWindow;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudio;




