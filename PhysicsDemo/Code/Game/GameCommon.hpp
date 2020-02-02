#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

constexpr float CLIENT_ASPECT = 16.f/9.f; 
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern InputSystem* g_theInput;




