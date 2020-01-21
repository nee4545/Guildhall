#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"

constexpr float CLIENT_ASPECT = 16.f/9.f; // We are requesting a 2:1 aspect (square) window area
extern RenderContext* g_theRenderer;
extern App* g_theapp;
extern InputSystem* g_theInput;

constexpr int WORLD_SIZE_X = 30;
constexpr int WORLD_SIZE_Y = 30;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;
constexpr float UI_XMAX = 1600.f;
constexpr float UI_YMaX = 800.f;

constexpr float PLAYER_SPEED = 2.6f;
constexpr float AI_TYPE1_SPEED = 3.f;
constexpr float AI_TYPE2_SPEED = 3.5f;
constexpr float AI_TYPE3_SPEED = 7.f;



