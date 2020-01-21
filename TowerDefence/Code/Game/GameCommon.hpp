#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

constexpr float CLIENT_ASPECT = 16.f/9.f; 
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern InputSystem* g_theInput;



constexpr int MAX_DEBRIS= 1000;

constexpr float UI_XMAX=1600.f;
constexpr float UI_YMaX=800.f;





