#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

class Window;

constexpr float CLIENT_ASPECT = 16.f/9.f; // We are requesting a 2:1 aspect (square) window area
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern Window* g_theWindow;
extern InputSystem* g_theInput;




